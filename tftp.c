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

#include "tftp.h"

//================================================================================
//
//  Packet_init 
//
//  Initializes a Packet struct to all zeroes.
//
//================================================================================
void Packet_init(Packet * thisP, unsigned short opcode)
{
    switch(opcode) {
        case 1:
        case 2:
            memset(thisP, 0, sizeof(RWRQ_Packet));
            break;
        case 3:
            memset(thisP, 0, sizeof(DATA_Packet));
            break;
        case 4:
            memset(thisP, 0, sizeof(ACK_Packet));
            break;
        case 5:
            memset(thisP, 0, sizeof(ERROR_Packet));
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
void RWRQ_Packet_construct_msg(Packet * thisP, unsigned short opcode, char * message)
{
    RWRQ_Packet * packet = (RWRQ_Packet *) &thisP;
    packet->opcode = opcode;        
    strcpy(packet->filename, read_message_filename(message));
    strcpy(packet->mode, read_message_mode(message));
}

void RWRQ_Packet_construct(Packet * thisP, unsigned short opcode, char * fname, char * mode)
{
    RWRQ_Packet * packet = (RWRQ_Packet *) &thisP;
    packet->opcode = opcode;        
    strcpy(packet->filename, fname);
    strcpy(packet->mode, mode);
}

//================================================================================
//
//  DATA_Packet_construct 
//
//================================================================================
void DATA_Packet_construct_msg(Packet * thisP, unsigned short opcode, char * message)
{
    DATA_Packet * packet = (DATA_Packet *) &thisP;
    packet->opcode = opcode;        
    packet->block_num = read_message_block_num(message);        
    memcpy(packet->data, read_message_data(message), DATA_SIZE);

}

void DATA_Packet_construct(Packet * thisP, unsigned short opcode, unsigned short b_num, char * data)
{
    DATA_Packet * packet = (DATA_Packet *) &thisP;
    packet->opcode = opcode;        
    packet->block_num = b_num;        
    strcpy(packet->data, data);
}

//================================================================================
//
//  ACK_Packet_construct 
//
//================================================================================
void ACK_Packet_construct_msg(Packet * thisP, unsigned short opcode, char * message)
{
    ACK_Packet * packet = (ACK_Packet *) &thisP;
    packet->opcode = opcode;        
    packet->block_num = read_message_block_num(message);        

}

void ACK_Packet_construct(Packet * thisP, unsigned short opcode, unsigned short b_num)
{
    ACK_Packet * packet = (ACK_Packet *) &thisP;
    packet->opcode = opcode;        
    packet->block_num = b_num;        
}

//================================================================================
//
//  ERROR_Packet_construct 
//
//================================================================================
void ERROR_Packet_construct_msg(Packet * thisP, unsigned short opcode, char * message)
{
    ERROR_Packet * packet = (ERROR_Packet *) &thisP;
    packet->opcode = opcode;        
    packet->error_code = read_message_error_code(message);        
    strcpy(packet->error_message, read_message_error_msg(message));

}

void ERROR_Packet_construct(Packet * thisP, unsigned short opcode, unsigned short e_code, char * error_msg)
{
    ERROR_Packet * packet = (ERROR_Packet *) &thisP;
    packet->opcode = opcode;        
    packet->error_code = e_code;        
    strcpy(packet->error_message, error_msg);
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

    Packet * new_packet;
    switch(opcode) {
        case 1:
        case 2:
            new_packet = malloc(sizeof(RWRQ_Packet));
            RWRQ_Packet_construct_msg(new_packet, opcode, message);
            break;
        case 3:
            new_packet = malloc(sizeof(DATA_Packet));
            DATA_Packet_construct_msg(new_packet, opcode, message);
            break;
        case 4:
            new_packet = malloc(sizeof(ACK_Packet));
            ACK_Packet_construct_msg(new_packet, opcode, message);
            break;
        case 5:
            new_packet = malloc(sizeof(ERROR_Packet));
            ERROR_Packet_construct_msg(new_packet, opcode, message);
            break;
        default:
            break;
    }

    return new_packet;

}


//================================================================================
//
//  read_message_opcode
//
//================================================================================
unsigned short read_message_opcode(char * message)
{
    unsigned short opcode;

    opcode = (unsigned short) (message[0] << 8) + message[1];
    //printf("opcode: %u\n", opcode);

    return opcode;
}

//================================================================================
//
//  read_message_block_num
//
//================================================================================
unsigned short read_message_block_num(char * message)
{
    unsigned short opcode;

    opcode = (unsigned short) (message[2] << 8) + message[3];
    //printf("opcode: %u\n", opcode);

    return opcode;
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
File_Container * file_open(char * filename, int opcode)
{
    char op;
    if(opcode == OP_RRQ)
        op = 'r';
    else if(opcode == OP_WRQ)
        op = 'w';
    else {
        printf("Error: invalid operation\n");
        exit(5);
    }

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
//  write_debug
//
//================================================================================
void write_debug(char * message)
{
    fprintf(stderr, "[DEBUG] %s\n", message);
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





