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
void process_message(char * message, int n);

//================================================================================
//
//  main
//
//================================================================================
void main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    struct sockaddr pcli_addr;
    int    sock_fd, n, clilen, i;
    char   mesg[MESSAGE_SIZE];

    // Display startup message
    printf("Group #06 Server\n");
    printf("Members: James Mack\n");

    // Create local socket
    sock_fd = setup_socket(INADDR_ANY, SERVER_PORT);


    // Main loop
    while(1) {
        clilen = sizeof(struct sockaddr);

        n = recvfrom(sock_fd, mesg, MESSAGE_SIZE, 0, &pcli_addr, &clilen);

        if(n < 0) {
            printf("recvfrom error\n");
            exit(3);
        }
        else {
            printf("request received\n");
            process_message(mesg, n);

        }

        if(sendto(sock_fd, mesg, n, 0, &pcli_addr, clilen) != n) {
            printf("sendto error\n");
            exit(4);
        }
    }
}

//================================================================================
//
//  process_message 
//
//================================================================================
void process_message(char * message, int n)
{
    Packet * packet = create_packet_from_message(message);
    memcpy(packet->message, message, n);
    //print_packet(packet);
    //unsigned short opcode = read_message_opcode(message);
    //printf("\topcode: %u\n", opcode);

}
