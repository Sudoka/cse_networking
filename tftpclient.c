//==============================================================================
//
//          tftpclient.c
//
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "tftp.h"
char test_packet[] = {0x00, 0x04, 0x00, 0x01};

char * filename = NULL;
int current_state;
int current_block;

int process_cl_args(int argc, char *argv[]);

//================================================================================
//
//  main
//
//================================================================================
void main(int argc, char *argv[])
{
    unsigned short client_op;
    int sock_fd, num_bytes, op_complete, data_size, bytes_received, last_packet_sent;
    char file_op;
    struct sockaddr_in serv_addr, cli_addr;
    char send_message[MESSAGE_SIZE], receive_message[MESSAGE_SIZE];
    Packet * packet = NULL;
    File_Container * transfer_file;
    
    client_op = process_cl_args(argc, argv);

    // Display init message
    printf("Group #06 Client\n");
    printf("Members: James Mack\n");
    printf("===================\n\n");


    // Create local socket
    sock_fd = setup_socket(INADDR_ANY, 0);

    // Outgoing
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    serv_addr.sin_port        = htons(SERVER_PORT);
    
    // Setup transfer file
    
    if(client_op == OP_RRQ) {
        file_op = 'a';
    }
    else if(client_op == OP_WRQ) {
        file_op = 'r';
    }
    transfer_file = file_open(filename, file_op);

    // Setup request message
    packet = Packet_init(client_op);
    RWRQ_Packet_construct(packet, client_op, filename, MODE);
    Packet_set_message(packet);

    // Send request message to server
    //sendto(sock_fd, packet->message, MESSAGE_SIZE, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    send_packet(packet, sock_fd, (struct sockaddr *) &serv_addr);
    op_complete = 0;
    current_block = 0;

    // Set state STATE_REQUEST_SENT
    current_state = STATE_REQUEST_SENT;
    last_packet_sent = 0;

    free(packet);
    packet = NULL;

    // Receive response from server
    while(!op_complete) {
        bytes_received = recvfrom(sock_fd, receive_message, MESSAGE_SIZE, 0, NULL, NULL);

        if(bytes_received < 0) {
            printf("recvfrom error\n");
            exit(4);
        }

        // Process response
        packet = create_packet_from_message(receive_message, bytes_received);
        printf("received:\t");
        Packet_display_string(packet);
        printf("\n");

        switch(packet->opcode) {
            case 1: // RRQ
                // INVALID STATE - client does not receive read requests
                break;

            case 2: // WRQ
                // INVALID STATE - client does not receive write requests 
                break;

            case 3: // DATA
                
                data_size = bytes_received - DATA_OFFSET;

                current_block = ((DATA_Packet *) packet)->block_num;
                memcpy(transfer_file->current_data, ((DATA_Packet *)packet)->data, data_size);

                num_bytes = file_write_next(transfer_file, data_size);

                // Send ACK for this packet
                free(packet);
                packet = Packet_init(OP_ACK);
                ACK_Packet_construct(packet, OP_ACK, current_block);
                Packet_set_message(packet);
                send_packet(packet, sock_fd, &serv_addr);

                if(data_size < DATA_SIZE) {
                    op_complete = 1;
                    current_state = STATE_COMPLETE;
                    file_close(transfer_file);
                }
                else {
                    current_state = STATE_WAITING_ACK; 
                }

                break;

            case 4: // ACK
                // Check that ACK was for the last block sent
                if((((ACK_Packet *)packet)->block_num == 0) && (current_block != ((ACK_Packet *)packet)->block_num)) {
                    // ACK not for last block sent or request packet
                    printf("block_num: %d\tcurrent_block: %d\n", ((ACK_Packet *)packet)->block_num , current_block);
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
                    op_complete = 1;
                }

                // Send next packet of DATA
                packet = Packet_init(OP_DATA);
                DATA_Packet_construct(packet, OP_DATA, ++current_block, transfer_file->current_data, num_bytes);
                Packet_set_message(packet);
                send_packet(packet, sock_fd, &serv_addr);
                current_state = STATE_WAITING_ACK;

                break;
            case 5: // ERROR
                break;

            default:
                break;
        }

        free(packet);
        packet = NULL;

        // update File_Container current_data
        memset(transfer_file->current_data, 0, num_bytes);
        //memcpy();
    }

    if(last_packet_sent) {
        printf("file complete\n");
    }

    close(sock_fd);
    exit(0);
}

//================================================================================
//
//  process_cl_args
//
//================================================================================
int process_cl_args(int argc, char *argv[])
{
    int opt, opcode;
    opterr = 0;

    // Process command line arguments
    while ((opt = getopt (argc, argv, "r:w:")) != -1) {
        switch(opt) {
            case 'r':
                opcode = OP_RRQ;
                filename = optarg;
                break;
            case 'w':
                opcode = OP_WRQ;
                filename = optarg;
                break;
            case '?':
                if(optopt == 'r') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if(optopt == 'w') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if(isprint(optopt)) {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                }
                else {
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                exit(0);
                break;
        }
    }

    return opcode;
}


