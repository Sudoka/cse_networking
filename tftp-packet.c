//==============================================================================
//
//          tftp-packet.c
//
//==============================================================================

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tftp-packet.h"

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
void RWRQ_Packet_construct_msg(RWRQ_Packet * thisP, unsigned short opcode, char * message)
{
    thisP->opcode = opcode;        
    strcpy(thisP->filename, read_message_filename(message));
    strcpy(thisP->mode, read_message_mode(message));
}

void RWRQ_Packet_construct(RWRQ_Packet * thisP, unsigned short opcode, char * fname, char * mode)
{
    thisP->opcode = opcode;        
    strcpy(thisP->filename, fname);
    strcpy(thisP->mode, mode);
}

//================================================================================
//
//  DATA_Packet_construct 
//
//================================================================================
void DATA_Packet_construct_msg(DATA_Packet * thisP, unsigned short opcode, char * message)
{
    thisP->opcode = opcode;        
    thisP->block_num = read_message_block_num(message);        
    memcpy(thisP->data, read_message_data(message), DATA_SIZE);

}

void DATA_Packet_construct(DATA_Packet * thisP, unsigned short opcode, unsigned short b_num, char * data)
{
    thisP->opcode = opcode;        
    thisP->block_num = b_num;        
    strcpy(thisP->data, data);
}

//================================================================================
//
//  ACK_Packet_construct 
//
//================================================================================
void ACK_Packet_construct_msg(ACK_Packet * thisP, unsigned short opcode, char * message)
{
    thisP->opcode = opcode;        
    thisP->block_num = read_message_block_num(message);        

}

void ACK_Packet_construct(ACK_Packet * thisP, unsigned short opcode, unsigned short b_num)
{
    thisP->opcode = opcode;        
    thisP->block_num = b_num;        
}

//================================================================================
//
//  ERROR_Packet_construct 
//
//================================================================================
void ERROR_Packet_construct_msg(ERROR_Packet * thisP, unsigned short opcode, char * message)
{
    thisP->opcode = opcode;        
    thisP->error_code = read_message_error_code(message);        
    strcpy(thisP->error_message, read_message_error_msg(message));

}

void ERROR_Packet_construct(ERROR_Packet * thisP, unsigned short opcode, unsigned short e_code, char * error_msg)
{
    thisP->opcode = opcode;        
    thisP->error_code = e_code;        
    strcpy(thisP->error_message, error_msg);
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
            RWRQ_Packet_construct_msg( (RWRQ_Packet *) new_packet, opcode, message);
            break;
        case 3:
            new_packet = malloc(sizeof(DATA_Packet));
            DATA_Packet_construct_msg( (DATA_Packet *) new_packet, opcode, message);
            break;
        case 4:
            new_packet = malloc(sizeof(ACK_Packet));
            ACK_Packet_construct_msg( (ACK_Packet *) new_packet, opcode, message);
            break;
        case 5:
            new_packet = malloc(sizeof(ERROR_Packet));
            ERROR_Packet_construct_msg( (ERROR_Packet *) new_packet, opcode, message);
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

//================================================================================
//
//  process_cl_args 
//
//================================================================================
/*
void process_cl_args(int argc, char **argv)
{
    char * r_value = NULL;
    char * w_value = NULL;
    int index;
    int opt;
     
    opterr = 0;
     
    while ((opt = getopt (argc, argv, "r:w:")) != -1)
        switch (opt) {
            case 'r':
                r_value = optarg;
                break;
            case 'w':
                w_value = optarg;
                break;
            case '?':
                if(optopt == 'r') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                if(optopt == 'w') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if(isprint(optopt)) {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                }
                else {
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
            default:
        }
    }
     
    printf ("aflag = %d, bflag = %d, cvalue = %s\n", aflag, bflag, cvalue);
     
    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
        return 0;
    }
}

*/


