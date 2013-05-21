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
void dg_echo(int sockfd);
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
    int    sockfd, n, clilen, i;
    char   mesg[MESSAGE_SIZE];

    // Display startup message
    printf("Group #06 Server\n");
    printf("Members: James Mack\n");

    // Create local socket
    sockfd = setup_socket(INADDR_ANY, SERVER_PORT);

    // Main loop
    while(1) {
        clilen = sizeof(struct sockaddr);

        n = recvfrom(sockfd, mesg, MESSAGE_SIZE, 0, &pcli_addr, &clilen);

        if(n < 0) {
            printf("recvfrom error\n");
            exit(3);
        }
        else {
            printf("request received: WRQ filename: fubar.txt\n", n);
            process_message(mesg, n);

        }

        if(sendto(sockfd, mesg, n, 0, &pcli_addr, clilen) != n) {
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
