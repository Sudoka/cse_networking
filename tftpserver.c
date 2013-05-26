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
void process_message(char * message, int n, int sock_fd, struct sockaddr * cli_addr)
{
    int num_bytes;
    Packet * packet = create_packet_from_message(message);
    Packet * response_packet;

    printf("packet received: ");
    Packet_display_string(packet);
    printf("\n");

    switch(packet->opcode) {
        case 1:
        // Begin new file transmission
            
            // Check for ready state
            if(current_state != STATE_READY) {
                printf("not able to begin new request\n");
                return;
            }

            // Open file for reading
            transfer_file = file_open(((RWRQ_Packet *)packet)->filename, 'r');
            current_block = 0;

            // Send first packet of DATA
            num_bytes = file_read_next(transfer_file); 
            response_packet = Packet_init(OP_DATA);
            DATA_Packet_construct(response_packet, OP_DATA, ++current_block, transfer_file->current_data);
            Packet_set_message(response_packet);
            send_packet(response_packet, sock_fd, cli_addr);
            current_state = STATE_WAITING_ACK;
            
            break;
        case 2:
        // Begin new file reception
            //if(current_state == STATE_READY) {
                transfer_file = file_open(((RWRQ_Packet *)packet)->filename, 'w');
                current_block = 0;
            //}

            // Send ACK for this packet
            response_packet = Packet_init(OP_ACK);
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
            file_close(transfer_file);

            break;
        case 4:
        // Received next ACK, send next DATA
            // Check that ACK was for the last block sent
            if(current_block != ((ACK_Packet *)packet)->block_num) {
                // ACK not for last block sent
                return;
            }

            // Check if file is done
            if(file_bytes_remaining(transfer_file) <= 0) {
                printf("file complete\n");
                return;
            }
            // Send next packet of DATA
            num_bytes = file_read_next(transfer_file); 
            response_packet = Packet_init(OP_DATA);
            DATA_Packet_construct(response_packet, OP_DATA, ++current_block, transfer_file->current_data);
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
    free(response_packet);
    if(DEBUG) printf("packet processed\n");
}
