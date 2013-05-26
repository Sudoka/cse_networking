//==============================================================================
//
//          tftp.c
//
//==============================================================================

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>

#include "tftp.h"

//================================================================================
//
//  Packet_init 
//
//  Initializes a Packet struct to all zeroes.
//
//================================================================================
Packet * Packet_init(unsigned short opcode)
{
    //if(DEBUG) printf("[DEBUG] Packet_init(%u)\n", opcode);
    Packet * packet = NULL;

    switch(opcode) {
        case 1:
        case 2:
            packet = (Packet *) malloc(sizeof(RWRQ_Packet));
            memset(packet, 0, sizeof(RWRQ_Packet));
            break;
        case 3:
            packet = (Packet *) malloc(sizeof(DATA_Packet));
            memset(packet, 0, sizeof(DATA_Packet));
            break;
        case 4:
            packet = (Packet *) malloc(sizeof(ACK_Packet));
            memset(packet, 0, sizeof(ACK_Packet));
            break;
        case 5:
            packet = (Packet *) malloc(sizeof(ERROR_Packet));
            memset(packet, 0, sizeof(ERROR_Packet));
            break;
        default:
            break;
    }

    return packet;
}

//================================================================================
//
//  Packet_set_message 
//
//================================================================================
void Packet_set_message(Packet * packet)
{
    //printf("opcode before conversion: %u\n", packet->opcode);
    *(unsigned short *)&(packet->message) = htons(packet->opcode);
    //printf("opcode after conversion: %u\n", htons(packet->opcode));

    switch(packet->opcode) {
        case 1:
        case 2:
            memcpy(&packet->message[2], ((RWRQ_Packet *)packet)->filename, sizeof(((RWRQ_Packet *)packet)->filename));
            break;
        case 3:
            *(unsigned short *)&(packet->message[2]) = htons(((DATA_Packet *)packet)->block_num);
            memcpy(&packet->message[4], ((DATA_Packet *)packet)->data, sizeof(((DATA_Packet *)packet)->data));
            break;
        case 4:
            *(unsigned short *)&(packet->message[2]) = htons(((ACK_Packet *)packet)->block_num);
            break;
        case 5:
            break;
        default:
            break;
    }
}

//================================================================================
//
//  Packet_display_string
//
//================================================================================
void Packet_display_string(Packet * packet)
{
    switch(packet->opcode) {
        case 1:
            printf("RRQ - filename: %s", ((RWRQ_Packet *)packet)->filename);
            break;
        case 2:
            printf("WRQ - filename: %s", ((RWRQ_Packet *)packet)->filename);
            break;
        case 3:
            printf("DATA - block #: %d", ((DATA_Packet *)packet)->block_num);
            break;
        case 4:
            printf("ACK - block #: %d", ((ACK_Packet *)packet)->block_num);
            break;
        case 5:
            printf("ERROR - error_code: %d - error_message: %s", ((ERROR_Packet *)packet)->error_code, ((ERROR_Packet *)packet)->error_message);
            break;
        default:
            break;
    }
}



//================================================================================
//
//  RWRQ_Packet_construct 
//
//================================================================================
void RWRQ_Packet_construct_msg(Packet * packet, unsigned short opcode, char * message)
{
    //printf("Creating RWRQ_Packet\n\topcode: %u\n\tfilename: %s\n", opcode, read_message_filename(message));
    packet->opcode = opcode;        
    strcpy(((RWRQ_Packet *) packet)->filename, read_message_filename(message));
    strcpy(((RWRQ_Packet *) packet)->mode, read_message_mode(message));
    strcpy(packet->message, message);
}

void RWRQ_Packet_construct(Packet * packet, unsigned short opcode, char * fname, char * mode)
{
    //printf("Creating RWRQ_Packet\n\topcode: %u\n\tfilename: %s\n", opcode, fname);
    packet->opcode = opcode;        
    strcpy(((RWRQ_Packet *) packet)->filename, fname);
    strcpy(((RWRQ_Packet *) packet)->mode, mode);
}

//================================================================================
//
//  DATA_Packet_construct 
//
//================================================================================

void DATA_Packet_construct_msg(Packet * packet, unsigned short opcode, char * message)
{
    packet->opcode = opcode;        
    ((DATA_Packet *) packet)->block_num = read_message_block_num(message);        
    memcpy(((DATA_Packet *) packet)->data, read_message_data(message), DATA_SIZE);

}

void DATA_Packet_construct(Packet * packet, unsigned short opcode, unsigned short b_num, char * data)
{
    packet->opcode = opcode;        
    ((DATA_Packet *) packet)->block_num = b_num;
    memcpy(((DATA_Packet *) packet)->data, data, DATA_SIZE);
}

//================================================================================
//
//  ACK_Packet_construct 
//
//================================================================================
void ACK_Packet_construct_msg(Packet * packet, unsigned short opcode, char * message)
{
    packet->opcode = opcode;        
    ((ACK_Packet *) packet)->block_num = read_message_block_num(message);        

}

void ACK_Packet_construct(Packet * packet, unsigned short opcode, unsigned short b_num)
{
    packet->opcode = opcode;        
    ((ACK_Packet *) packet)->block_num = b_num;
}

//================================================================================
//
//  ERROR_Packet_construct 
//
//================================================================================
void ERROR_Packet_construct_msg(Packet * packet, unsigned short opcode, char * message)
{
    packet->opcode = opcode;        
    ((ERROR_Packet *)packet)->error_code = read_message_error_code(message);        
    strcpy(((ERROR_Packet *)packet)->error_message, read_message_error_msg(message));

}

void ERROR_Packet_construct(Packet * packet, unsigned short opcode, unsigned short e_code, char * error_msg)
{
    packet->opcode = opcode;        
    ((ERROR_Packet *)packet)->error_code = e_code;        
    strcpy(((ERROR_Packet *)packet)->error_message, error_msg);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//================================================================================
//
//  create_packet_from_message
//
//================================================================================
Packet * create_packet_from_message(char * message)
{
    unsigned short opcode = read_message_opcode(message);

    Packet * packet;
    switch(opcode) {
        case 1:
        case 2:
            packet = Packet_init(opcode);
            RWRQ_Packet_construct_msg(packet, opcode, message);
            break;
        case 3:
            packet = Packet_init(opcode);
            DATA_Packet_construct_msg(packet, opcode, message);
            break;
        case 4:
            packet = Packet_init(opcode);
            ACK_Packet_construct_msg(packet, opcode, message);
            break;
        case 5:
            packet = Packet_init(opcode);
            ERROR_Packet_construct_msg(packet, opcode, message);
            break;
        default:
            printf("opcode error: opcode = %d\n", opcode);
            exit(1);
            break;
    }

    return packet;

}


//================================================================================
//
//  read_message_opcode
//
//================================================================================
unsigned short read_message_opcode(char * message)
{
    unsigned short opcode;

    opcode = *((unsigned short *) &message[0]);
    opcode = ntohs(opcode);

    if(DEBUG) printf("[DEBUG] read_message_opcode() - opcode: %u\n", opcode);

    return opcode;
}

//================================================================================
//
//  read_message_block_num
//
//================================================================================
unsigned short read_message_block_num(char * message)
{
    unsigned short block_num;

    block_num = *((unsigned short *) &message[2]);
    block_num = ntohs(block_num);
    if(DEBUG) printf("[DEBUG] read_message_block_num() - block_num: %u\n", block_num);

    return block_num;
}

//================================================================================
//
//  read_message_error_code
//
//================================================================================
unsigned short read_message_error_code(char * message)
{
    return read_message_block_num(message);
}

//================================================================================
//
//  read_message_filename
//
//================================================================================
char * read_message_filename(char * message)
{
    char * filename = malloc(FILENAME_LENGTH + 1);
    
    strcpy(filename, message + 2);

    //printf("filename: %s\n", filename);

    return filename;

}

//================================================================================
//
//  read_message_mode
//
//================================================================================
char * read_message_mode(char * message)
{
    char * mode = malloc(FILENAME_LENGTH + 1);
    
    int offset; 
    offset = strlen(message + 2) + 2 + 1;
    //printf("mode-offset: %d\n", offset);
    strcpy(mode, message + offset);

    //printf("mode: %s\n", mode);

    return mode;
}

//================================================================================
//
//  read_message_data
//
//================================================================================
char * read_message_data(char * message)
{
    char * data = malloc(DATA_SIZE);
    
    memcpy(data, message + (MESSAGE_SIZE - DATA_SIZE), DATA_SIZE);

    //printf("data: %s\n", data);

    return data;
}

//================================================================================
//
//  read_message_error_msg
//
//================================================================================
char * read_message_error_msg(char * message)
{
    char * error_msg = malloc(ERROR_LENGTH + 1);
    
    int offset; 
    offset = strlen(message + 2) + 2 + 1;
    strcpy(error_msg, message + (MESSAGE_SIZE - DATA_SIZE));

    //printf("error_msg: %s\n", error_msg);

    return error_msg;
}

//================================================================================
//
//  file_open
//
//================================================================================
File_Container * file_open(char * filename, char op)
{
    /*
    char op;
    if(opcode == OP_RRQ)
        op = 'r';
    else if(opcode == OP_WRQ)
        op = 'w';
    else {
        printf("Error: invalid operation\n");
        exit(5);
    }
    */

    File_Container * new_file = malloc(sizeof(File_Container));
    memset(new_file, 0, sizeof(File_Container));

    new_file->fp = fopen(filename, &op);
    new_file->count = 0; 

    if(new_file->fp == NULL) {
        printf("Failed to open file %s\n", filename);
        exit(1);
    }
    
    return new_file;
}

//================================================================================
//
//  file_close
//
//================================================================================
void file_close(File_Container * this_file)
{
    fclose(this_file->fp);

    free(this_file);
}

//================================================================================
//
//  file_read_next
//
//================================================================================
int file_read_next(File_Container * this_file)
{
    memset(this_file->current_data, 0, sizeof(this_file->current_data));

    return fread(this_file->current_data, sizeof(char), DATA_SIZE, this_file->fp);

}

//================================================================================
//
//  file_write_next
//
//================================================================================
int file_write_next(File_Container * this_file, int length)
{
    return fwrite(this_file->current_data, sizeof(char), length, this_file->fp);
}

//================================================================================
//
//  file_get_size
//
//================================================================================
int file_get_size(File_Container * this_file)
{
    int current, size;

    current = ftell(this_file->fp);
    fseek(this_file->fp, 0, SEEK_END);
    size = ftell(this_file->fp);

    fseek(this_file->fp, current, SEEK_SET);

    return size;
}

//================================================================================
//
//  file_bytes_remaining
//
//================================================================================
int file_bytes_remaining(File_Container * this_file)
{
    int current, size;

    current = ftell(this_file->fp);
    size = file_get_size(this_file);


    return size - current;
}


//================================================================================
//
//  print_packet
//
//================================================================================
void print_packet(Packet * packet)
{
    switch(packet->opcode) {
        case 1:
        case 2:
            printf("RWRQ_Packet\n");
            printf("\topcode:\t\t%u\n", ((RWRQ_Packet *) packet)->opcode);
            printf("\tmessage:\t%s\n", ((RWRQ_Packet *) packet)->message);
            printf("\tfilename:\t%s\n", ((RWRQ_Packet *) packet)->filename);
            printf("\tmode:\t\t%s\n", ((RWRQ_Packet *) packet)->mode);
            break;
        case 3:
            printf("DATA_Packet\n");
            printf("\topcode:\t\t%u\n", ((DATA_Packet *) packet)->opcode);
            printf("\tmessage:\t%s\n", ((DATA_Packet *) packet)->message);
            printf("\tblock_num:\t%u\n", ((DATA_Packet *) packet)->block_num);
            printf("\tdata:\t\t%s\n", ((DATA_Packet *) packet)->data);
            break;
        case 4:
            printf("ACK_Packet\n");
            printf("\topcode:\t\t%u\n", ((ACK_Packet *) packet)->opcode);
            printf("\tmessage:\t%s\n", ((ACK_Packet *) packet)->message);
            printf("\tblock_num:\t%u\n", ((ACK_Packet *) packet)->block_num);
            break;
        case 5:
            printf("ERROR_Packet\n");
            printf("\topcode:\t\t%u\n", ((ERROR_Packet *) packet)->opcode);
            printf("\tmessage:\t%s\n", ((ERROR_Packet *) packet)->message);
            printf("\terror_code:\t%u\n", ((ERROR_Packet *) packet)->error_code);
            printf("\terror_message:\t%s\n", ((ERROR_Packet *) packet)->error_message);
            break;
        default:
            break;

    }
}

//================================================================================
//
//  setup_socket
//
//================================================================================
int setup_socket(char * address, int port)
{
    int sock_fd, bind_result;
    struct sockaddr_in addr;

    // Create local socket
    if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("can't open datagram socket\n");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    if(address == 0) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else {
        addr.sin_addr.s_addr = inet_addr(address);
    }
    addr.sin_port        = htons(port);

    if(bind(sock_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        printf("can't bind local address\n");
        exit(2);
    }

    return sock_fd;
}

//================================================================================
//
//  send_packet
//
//================================================================================
int send_packet(Packet * packet, int sock_fd, struct sockaddr * serv_addr)
{
    int bytes = 0;
    printf("sending: ");
    Packet_display_string(packet);
    printf("\n");


    bytes = sendto(sock_fd, packet->message, MESSAGE_SIZE, 0, serv_addr, sizeof(*serv_addr));

    if(DEBUG) printf("bytes sent: %d\n", bytes);

    if(bytes != MESSAGE_SIZE) {
        printf("sendto error: %s\n", strerror(errno));
        exit(4);
    }

    return bytes;


}

/*
    switch(opcode) {
        case 1:
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;


    }


*/
