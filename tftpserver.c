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
void write_debug(char * message);
unsigned short read_message_opcode(char * message);
Packet * create_packet_from_message(char * message);
void process_message(char * message, int n);
void server_init();

//================================================================================
//
//  main
//
//================================================================================
void main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serv_addr;
    printf("Group #06 Server\n");
    printf("Members: James Mack\n");

    // Create local socket
    //sockfd = setup_socket(INADDR_ANY, SERVER_PORT);
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("can't open datagram socket\n");
        exit(1); 
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERVER_PORT);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) { 
        printf("can't bind local address\n");
        exit(2);
    }

    dg_echo(sockfd);
}

//================================================================================
//
//  dg_echo 
//
//================================================================================
void dg_echo(int sockfd)
{
    struct sockaddr pcli_addr;
    
    int    n, clilen, i;
    char   mesg[MESSAGE_SIZE];

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

        printf("request completed\n\n");
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
