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

#include "tftp-packet.h"

#define SERV_UDP_PORT   12345
#define SERV_HOST_ADDR  "127.0.0.1"

#define DEBUG 0

char *progname;

#define MAXMESG 2048            // Size of maximum message to received.

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

    server_init();

    progname = argv[0];

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("%s: can't open datagram socket\n", progname);
        exit(1); 
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    serv_addr.sin_port = htons(SERV_UDP_PORT);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) { 
        printf("%s: can't bind local address\n",progname);
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
    char   mesg[MAXMESG];

    for( ; ; ) {

        clilen = sizeof(struct sockaddr);

        n = recvfrom(sockfd, mesg, MAXMESG, 0, &pcli_addr, &clilen);

        if(n < 0) {
            printf("%s: recvfrom error\n",progname);
            exit(3);
        }
        else {
            printf("request received: WRQ filename: fubar.txt\n", n);
            process_message(mesg, n);

        }

        if(sendto(sockfd, mesg, n, 0, &pcli_addr, clilen) != n) {
            printf("%s: sendto error\n",progname);
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


void server_init()
{
    printf("Group #06 Server\n");
    printf("Members: James Mack\n");
    printf("tftp server running, waiting for requests...\n");

}

