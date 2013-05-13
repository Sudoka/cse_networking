//==============================================================================
//
//          tftpclient.c
//
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define SERV_UDP_PORT   12345
#define SERV_HOST_ADDR  "127.0.0.1"
#define MAXLINE         512             // Size of maximum message to send


char *progname;         // A pointer to the name of this program for error reporting
char test_packet1[] = {0x00, 0x01, 0x66, 0x6f, 0x6f, 0x2e, 0x74, 0x78, 0x74, 0x00, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x00};
char test_packet2[] = {0x00, 0x02, 0x62, 0x61, 0x72, 0x2e, 0x74, 0x78, 0x74, 0x00, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x00};
char test_packet3[] = {0x00, 0x03, 0x00, 0x01, 
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79,
    0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79, 0x58, 0x79
    };
char test_packet4[] = {0x00, 0x04, 0x00, 0x01};
char test_packet5[] = {0x00, 0x05, 0x00, 0x01, 0x46, 0x69, 0x6c, 0x65, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x2e};

void client_init()
{
    printf("Group #06 Client\n");
    printf("Member: James Mack\n");
}


void dg_cli(int sockfd, struct sockaddr * pserv_addr, int servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    //while(fgets(sendline, MAXLINE, stdin) != NULL) {


        //n = strlen(sendline); 
        
        n = sendto(sockfd, test_packet5, 516, 0, pserv_addr, servlen);
        /*
        if(sendto(sockfd, test_packet, 516, 0, pserv_addr, servlen) != n) {
            printf("%s: sendto error on socket\n",progname);
            exit(3);
        }
        */

        n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);

        if(n < 0) {
            printf("%s: recvfrom error\n",progname);
            exit(4);
        }
        
        recvline[n] = 0;
        fputs(recvline, stdout);
    //}
}


void main(int argc, char *argv[])
{
    int sockfd;
    
    struct sockaddr_in cli_addr, serv_addr;
    
    client_init();

    progname = argv[0];

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    
    serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
    serv_addr.sin_port        = htons(SERV_UDP_PORT);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("%s: can't open datagram socket\n",progname);
        exit(1);
    }

    bzero((char *) &cli_addr, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    cli_addr.sin_port = htons(0);
    
    if(bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
        printf("%s: can't bind local address\n",progname);
        exit(2);
    }

    dg_cli(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    close(sockfd);
    exit(0);
}

