#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_UDP_PORT   12345
#define SERV_HOST_ADDR  "127.0.0.1"

char *progname;

#define MAXMESG 2048            // Size of maximum message to received.

void dg_echo(int sockfd)
{
    struct sockaddr pcli_addr;
    
    int    n, clilen;
    char   mesg[MAXMESG];

    for( ; ; ) {

        clilen = sizeof(struct sockaddr);

        n = recvfrom(sockfd, mesg, MAXMESG, 0, &pcli_addr, &clilen);
        
        if(n < 0) {
            printf("%s: recvfrom error\n",progname);
            exit(3);
        }

        if(sendto(sockfd, mesg, n, 0, &pcli_addr, clilen) != n) {
            printf("%s: sendto error\n",progname);
            exit(4);
        }
    }
}

void main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serv_addr;

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