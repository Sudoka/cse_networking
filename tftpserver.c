//============================================================================== //
//          tftpserver.c
//
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tftp.h"

// Function prototypes
void process_message(char * message, int n, int sock_fd, struct sockaddr * cli_addr);

// Global Variables
unsigned short current_block;
int current_state;
int last_packet_sent;
File_Container * transfer_file;

//================================================================================
//
//  main
//
//================================================================================
void main(int argc, char *argv[])
{
    //struct sockaddr_in serv_addr;
    struct sockaddr * cli_addr;
    int    sock_fd, n, clilen, i;
    char   mesg[MESSAGE_SIZE];

    // Display startup message
    printf("Group #06 Server\n");
    printf("Members: James Mack\n");
    printf("===================\n\n");

    // Create local socket
    sock_fd = setup_socket(INADDR_ANY, SERVER_PORT);
    current_state = STATE_READY;

    clilen = sizeof(struct sockaddr);

    // Main loop
    while(1) {

        n = recvfrom(sock_fd, mesg, MESSAGE_SIZE, 0, cli_addr, &clilen);

        if(n < 0) {
            printf("recvfrom error\n");
            exit(3);
        }
        else {
            process_message(mesg, n, sock_fd, cli_addr);
        }
    }
}

//================================================================================
//
//  process_message 
//
//================================================================================
void process_message(char * message, int bytes_received, int sock_fd, struct sockaddr * cli_addr)
{
    int num_bytes;
    Packet * packet = create_packet_from_message(message, bytes_received);
    Packet * response_packet;

    if(DEBUG) printf("[DEBUG] process_message()\tbytes received: %d\n", bytes_received);

    printf("received:\t");
    Packet_display_string(packet);
    printf("\n");

    switch(packet->opcode) {
        case 1:
        // Begin new file transmission
            
            // Check for ready state
            if(current_state != STATE_READY) {
                printf("not able to begin new request\n");
                break;
            }

            // Open file for reading
            transfer_file = file_open(((RWRQ_Packet *)packet)->filename, 'r');
            current_block = 0;
            last_packet_sent = 0;

            // Send first packet of DATA
            num_bytes = file_read_next(transfer_file); 
            response_packet = Packet_init(OP_DATA);
            DATA_Packet_construct(response_packet, OP_DATA, ++current_block, transfer_file->current_data, num_bytes);
            Packet_set_message(response_packet);
            send_packet(response_packet, sock_fd, cli_addr);
            current_state = STATE_WAITING_ACK;
            
            break;
        case 2:
        // Begin new file reception
            //if(current_state == STATE_READY) {
                transfer_file = file_open(((RWRQ_Packet *)packet)->filename, 'a');
                current_block = 0;
            //}

            // Send ACK for this packet
            response_packet = Packet_init(OP_ACK);
            // TODO: Think this current_block needs incrementation
            ACK_Packet_construct(response_packet, OP_ACK, current_block);
            Packet_set_message(response_packet);
            send_packet(response_packet, sock_fd, cli_addr);

            break;
        case 3:
        // Received next DATA, add to file and send ACK
            if(current_state == STATE_READY) {
                current_block = ((DATA_Packet *) packet)->block_num;
                memcpy(transfer_file->current_data, ((DATA_Packet *)packet)->data, DATA_SIZE);

                num_bytes = file_write_next(transfer_file, DATA_SIZE);

            }
            // ACK sent for previous block

            // Send ACK for this packet
            response_packet = Packet_init(OP_ACK);
            ACK_Packet_construct(response_packet, OP_ACK, current_block);
            Packet_set_message(response_packet);
            send_packet(response_packet, sock_fd, cli_addr);

            // If last packet, close file
            if(bytes_received < (DATA_SIZE - DATA_OFFSET)) {
                file_close(transfer_file);
            }

            break;
        case 4:
        // Received next ACK, send next DATA
            // Check that ACK was for the last block sent
            if(current_block != ((ACK_Packet *)packet)->block_num) {
                // ACK not for last block sent
                break;
            }
            if(last_packet_sent) {
                if(file_bytes_remaining(transfer_file) != 0) {
                    printf("error: last_packet_sent flag set but bytes remain in file\n");
                }
                break;
            }

            // Check if file is done
            if(file_bytes_remaining(transfer_file) <= 0) {
                // send empty final packet
                num_bytes = 0;
                memset(transfer_file->current_data, 0, DATA_SIZE);    
            }
            else {
                // Send next packet of DATA
                num_bytes = file_read_next(transfer_file); 
            }

            if(num_bytes < DATA_SIZE) {
                last_packet_sent = 1;
            }

            response_packet = Packet_init(OP_DATA);
            DATA_Packet_construct(response_packet, OP_DATA, ++current_block, transfer_file->current_data, num_bytes);
            Packet_set_message(response_packet);
            send_packet(response_packet, sock_fd, cli_addr);
            current_state = STATE_WAITING_ACK;


            break;
        case 5:
            break;
        default:
            break;
    }

    free(packet);
    // TODO: Memory leak
    //free(response_packet);

    if(last_packet_sent) {
        printf("file complete\n");
    }

}
