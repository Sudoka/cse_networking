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
    if(DEBUG) printf("\t[DEBUG] Packet_init(%2u)\n", opcode);
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
    int offset;
    *(unsigned short *)&(packet->message) = htons(packet->opcode);
    if(DEBUG) printf("\t[DEBUG] Packet_set_message(%2u)\n", packet->opcode);
    
    switch(packet->opcode) {
        case 1:
        case 2:
            memcpy(&packet->message[2], ((RWRQ_Packet *)packet)->filename, sizeof(((RWRQ_Packet *)packet)->filename) + 1);
            offset = sizeof(packet->opcode) + strlen(((RWRQ_Packet *) packet)->filename) + 1;
            if(DEBUG) {
                printf("\t\toffset: %d\n", offset);
                printf("\t\tsizeof(packet->opcode): %d\n", sizeof(packet->opcode));
                printf("\t\tstrlen(packet->filename: %d\n", strlen(((RWRQ_Packet *) packet)->filename));
            }
            memcpy(&packet->message[offset], ((RWRQ_Packet *)packet)->mode, sizeof(((RWRQ_Packet *)packet)->mode) + 1);
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
            printf("RRQ\tfilename: %s\tsize: %d", ((RWRQ_Packet *)packet)->filename, packet->size);
            break;
        case 2:
            printf("WRQ\tfilename: %s\tsize: %d", ((RWRQ_Packet *)packet)->filename, packet->size);
            break;
        case 3:
            printf("DATA\tblock #: %d\t\tsize: %d", ((DATA_Packet *)packet)->block_num, packet->size);
            break;
        case 4:
            printf("ACK\tblock #: %d\t\tsize: %d", ((ACK_Packet *)packet)->block_num, packet->size);
            break;
        case 5:
            printf("ERROR\terror_code: %d\terror_message: %s\t\tsize: %d", ((ERROR_Packet *)packet)->error_code, ((ERROR_Packet *)packet)->error_message, packet->size);
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
    char * filename;
    char * mode;

    filename = read_message_filename(message);
    mode = read_message_mode(message, strlen(filename));

    packet->opcode = opcode;        
    strcpy(((RWRQ_Packet *) packet)->filename, filename);
    strcpy(((RWRQ_Packet *) packet)->mode, mode);
    strcpy(packet->message, message);
    packet->size = sizeof(packet->opcode) + strlen(((RWRQ_Packet *) packet)->filename) + strlen(((RWRQ_Packet *) packet)->mode) + 1 + 1;
    if(DEBUG) {
        printf("\t[DEBUG] Creating RWRQ_Packet\tsize: %d\n", packet->size);
        printf("\t\topcode: %2u\tsize: %d\n", opcode, sizeof(opcode));
        printf("\t\tfilename: %s\tsize: %d\n", filename, strlen(filename));
        printf("\t\tmode: %s\tsize: %d\n", mode, strlen(mode));
    }
}

void RWRQ_Packet_construct(Packet * packet, unsigned short opcode, char * filename, char * mode)
{
    packet->opcode = opcode;        
    strcpy(((RWRQ_Packet *) packet)->filename, filename);
    strcpy(((RWRQ_Packet *) packet)->mode, mode);
    packet->size = sizeof(opcode) + strlen(filename) + strlen(mode) + 1 + 1;

    if(DEBUG) {
        printf("\t[DEBUG] Creating RWRQ_Packet\tsize: %d\n", packet->size);
        printf("\t\topcode: %2u\tsize: %d\n", opcode, sizeof(opcode));
        printf("\t\tfilename: %s\tsize: %d\n", filename, strlen(filename));
        printf("\t\tmode: %s\tsize: %d\n", mode, strlen(mode));
    }
}

//================================================================================
//
//  DATA_Packet_construct 
//
//================================================================================

void DATA_Packet_construct_msg(Packet * packet, unsigned short opcode, char * message, int data_size)
{
    packet->opcode = opcode;        
    ((DATA_Packet *) packet)->block_num = read_message_block_num(message);        
    memcpy(((DATA_Packet *) packet)->data, read_message_data(message, data_size), data_size);

    packet->size = 2 + 2 + data_size;
}

void DATA_Packet_construct(Packet * packet, unsigned short opcode, unsigned short b_num, char * data, int size)
{
    packet->opcode = opcode;        
    ((DATA_Packet *) packet)->block_num = b_num;
    memcpy(((DATA_Packet *) packet)->data, data, size);

    packet->size = 2 + 2 + size;
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

    packet->size = 2 + 2;
}

void ACK_Packet_construct(Packet * packet, unsigned short opcode, unsigned short b_num)
{
    packet->opcode = opcode;        
    ((ACK_Packet *) packet)->block_num = b_num;
    packet->size = 2 + 2;
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

    packet->size = 2 + 2 + strlen(((ERROR_Packet *) packet)->error_message) + 1;
    

}

void ERROR_Packet_construct(Packet * packet, unsigned short opcode, unsigned short e_code, char * error_msg)
{
    packet->opcode = opcode;        
    ((ERROR_Packet *)packet)->error_code = e_code;        
    strcpy(((ERROR_Packet *)packet)->error_message, error_msg);

    packet->size = 2 + 2 + strlen(error_msg) + 1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//================================================================================
//
//  create_packet_from_message
//
//================================================================================
Packet * create_packet_from_message(char * message, int size)
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
            DATA_Packet_construct_msg(packet, opcode, message, size - 4);
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

    if(DEBUG) printf("\t[DEBUG] read_message_opcode()\topcode: %2u\n", opcode);

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
    if(DEBUG) printf("\t[DEBUG] read_message_block_num()\tblock_num: %2u\n", block_num);

    return block_num;
}

//================================================================================
//
//  read_message_error_code
//
//================================================================================
unsigned short read_message_error_code(char * message)
{
    unsigned short error_code;
    error_code = read_message_block_num(message);
    if(DEBUG) printf("\t[DEBUG] read_message_error_code()\terror_code: %2u\n", error_code);
    return error_code;
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

    if(DEBUG) printf("\t[DEBUG] read_message_filename\tfilename: %s\n", filename);

    return filename;

}

//================================================================================
//
//  read_message_mode
//
//================================================================================
char * read_message_mode(char * message, int filename_size)
{
    char * mode = malloc(FILENAME_LENGTH + 1);
    // TODO: Memory leak?
    
    int offset; 
    offset = filename_size + 2 + 1;
    strcpy(mode, message + offset);

    
    if(DEBUG) printf("\t[DEBUG] read_message_mode\tmode: %s\n", mode);

    return mode;
}

//================================================================================
//
//  read_message_data
//
//================================================================================
char * read_message_data(char * message, int data_size)
{
    char * data = malloc(data_size + 1);
    // TODO: Memory leak?
    int offset = MESSAGE_SIZE - DATA_SIZE; 
    if(!data_size) {
        data[0] = 0;
    }
    else {
        memcpy(data, message + (MESSAGE_SIZE - DATA_SIZE), data_size);
    }

    if(DEBUG) {
        printf("\t[DEBUG] read_message_data\n");
        printf("\t\toffset: %d\n", offset);
        printf("\t\tdata: %s\n", data);
    }

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
    // TODO change something here
    if(DEBUG) printf("\t[DEBUG] read_message_error_msg\terror_msg: %s\n", error_msg);

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
    if(DEBUG) printf("\t[DEBUG] opening file: %s\top: %c\n", filename, op);
    File_Container * new_file = malloc(sizeof(File_Container));
    memset(new_file, 0, sizeof(File_Container));

    new_file->fp = fopen(filename, &op);
    memcpy(new_file->filename, filename, strlen(filename) + 1);
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
    if(DEBUG) printf("\t[DEBUG] closing file: %s\n", this_file->filename);
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
    int bytes;
    memset(this_file->current_data, 0, sizeof(this_file->current_data));
    
    bytes = fread(this_file->current_data, sizeof(char), DATA_SIZE, this_file->fp);
    if(DEBUG) printf("\t[DEBUG]file_read_next()\tbytes read: %d\n", bytes);
    return bytes;
}

//================================================================================
//
//  file_write_next
//
//================================================================================
int file_write_next(File_Container * this_file, int length)
{
    int bytes;
    bytes = fwrite(this_file->current_data, sizeof(char), length, this_file->fp);
    this_file->current_size = bytes;

    if(DEBUG) printf("\t[DEBUG]file_write_next()\tbytes written: %d\n", bytes);
    return bytes;
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
            printf("\topcode:\t\t%2u\n", ((RWRQ_Packet *) packet)->opcode);
            printf("\tmessage:\t%s\n", ((RWRQ_Packet *) packet)->message);
            printf("\tfilename:\t%s\n", ((RWRQ_Packet *) packet)->filename);
            printf("\tmode:\t\t%s\n", ((RWRQ_Packet *) packet)->mode);
            break;
        case 3:
            printf("DATA_Packet\n");
            printf("\topcode:\t\t%2u\n", ((DATA_Packet *) packet)->opcode);
            printf("\tmessage:\t%s\n", ((DATA_Packet *) packet)->message);
            printf("\tblock_num:\t%2u\n", ((DATA_Packet *) packet)->block_num);
            printf("\tdata:\t\t%s\n", ((DATA_Packet *) packet)->data);
            break;
        case 4:
            printf("ACK_Packet\n");
            printf("\topcode:\t\t%2u\n", ((ACK_Packet *) packet)->opcode);
            printf("\tmessage:\t%s\n", ((ACK_Packet *) packet)->message);
            printf("\tblock_num:\t%2u\n", ((ACK_Packet *) packet)->block_num);
            break;
        case 5:
            printf("ERROR_Packet\n");
            printf("\topcode:\t\t%2u\n", ((ERROR_Packet *) packet)->opcode);
            printf("\tmessage:\t%s\n", ((ERROR_Packet *) packet)->message);
            printf("\terror_code:\t%2u\n", ((ERROR_Packet *) packet)->error_code);
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
    printf("sending:\t");
    Packet_display_string(packet);
    printf("\n");


    bytes = sendto(sock_fd, packet->message, packet->size, 0, serv_addr, sizeof(*serv_addr));

    if(DEBUG) printf("\t[DEBUG] send_packet()\tbytes sent: %d\n", bytes);

    if(bytes != packet->size) {
        printf("sendto error: %s\n", strerror(errno));
        exit(4);
    }

    return bytes;
}
