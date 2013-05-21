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

void dg_cli(int sockfd, struct sockaddr * pserv_addr, int servlen, char op, char * filename);


void main(int argc, char *argv[])
{
    int sockfd, index, opt;
    struct sockaddr_in serv_addr, cli_addr;
    char op;
    char * filename = NULL;

    opterr = 0;

    // Process command line arguments
    while ((opt = getopt (argc, argv, "r:w:")) != -1) {
        switch(opt) {
            case 'r':
                op = 'r';
                filename = optarg;
                break;
            case 'w':
                op = 'w';
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

    // Display init message
    printf("Group #06 Client\n");
    printf("Members: James Mack\n");

    // Create local socket
    //sockfd = setup_socket(INADDR_ANY, 0);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("can't open datagram socket\n");
        exit(1);
    }

    bzero((char *) &cli_addr, sizeof(cli_addr));
    cli_addr.sin_family      = AF_INET;
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cli_addr.sin_port        = htons(0);

    if(bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
        printf("can't bind local address\n");
        exit(2);
    }

    // Outgoing
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    serv_addr.sin_port        = htons(SERVER_PORT);

    dg_cli(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr), op, filename);

    close(sockfd);
    exit(0);
}

void dg_cli(int sockfd, struct sockaddr * pserv_addr, int servlen, char op, char * filename)
{
    int n, i;
    char sendline[MESSAGE_SIZE], recvline[MESSAGE_SIZE + 1];
    int op_complete = 0;

   // while(!op_complete) {
        
        printf("sending message\n");

        sendto(sockfd, test_packet, MESSAGE_SIZE, 0, pserv_addr, servlen);

        n = recvfrom(sockfd, recvline, MESSAGE_SIZE, 0, NULL, NULL);

        if(n < 0) {
            printf("recvfrom error\n");
            exit(4);
        }
        
        recvline[n] = 0;
        //fputs(recvline, stdout);
    //}
}



