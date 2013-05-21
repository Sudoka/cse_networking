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

char op;
char * filename = NULL;

int process_cl_args(int argc, char *argv[]);

//================================================================================
//
//  main
//
//================================================================================
void main(int argc, char *argv[])
{
    int sock_fd, bytes_received, opcode, op_complete;
    struct sockaddr_in serv_addr, cli_addr;
    char send_message[MESSAGE_SIZE], receive_message[MESSAGE_SIZE];
    Packet * packet;
    File_Container * transfer_file;
    
    opcode = process_cl_args(argc, argv);

    // Display init message
    printf("Group #06 Client\n");
    printf("Members: James Mack\n");

    // Create local socket
    sock_fd = setup_socket(INADDR_ANY, 0);

    // Outgoing
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    serv_addr.sin_port        = htons(SERVER_PORT);
    
    // Setup transfer file
    transfer_file = file_open(filename, opcode);

    // Setup request message
    Packet_init(packet, opcode);
    RWRQ_Packet_construct(packet, opcode, filename, MODE);

    // Send request message to server
    sendto(sock_fd, packet->message, MESSAGE_SIZE, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    op_complete = 0;

    // Receive response from server
    while(!op_complete) {
        bytes_received = recvfrom(sock_fd, receive_message, MESSAGE_SIZE, 0, NULL, NULL);

        if(bytes_received < 0) {
            printf("recvfrom error\n");
            exit(4);
        }

        // Process response
        //create_
            // update File_Container current_data
            memset(transfer_file->current_data, 0, bytes_received);
            //memcpy();

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


