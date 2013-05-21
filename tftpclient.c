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

void process_cl_args(int argc, char *argv[]);

//================================================================================
//
//  main
//
//================================================================================
void main(int argc, char *argv[])
{
    int sock_fd, n, i;
    struct sockaddr_in serv_addr, cli_addr;
    char sendline[MESSAGE_SIZE], recvline[MESSAGE_SIZE + 1];

    // Display init message
    printf("Group #06 Client\n");
    printf("Members: James Mack\n");

    // Create local socket
    sock_fd = setup_socket(INADDR_ANY, 0);

    // Outgoing
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    serv_addr.sin_port        = htons(SERVER_PORT);

    printf("sending message\n");

    sendto(sock_fd, test_packet, MESSAGE_SIZE, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    n = recvfrom(sock_fd, recvline, MESSAGE_SIZE, 0, NULL, NULL);

    if(n < 0) {
        printf("recvfrom error\n");
        exit(4);
    }
    
    recvline[n] = 0;

    close(sock_fd);
    exit(0);
}

//================================================================================
//
//  process_cl_args
//
//================================================================================
void process_cl_args(int argc, char *argv[])
{
    int opt;
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
}


