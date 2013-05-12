#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tftp-packet.h"

void hello_world()
{
    printf("hello, world\n");
}

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
void ERROR_Packet_construct(ERROR_Packet * thisP, unsigned short opcode, unsigned short e_code, char * error_msg)
{
    thisP->opcode = opcode;        
    thisP->error_code = e_code;        
    strcpy(thisP->error_message, error_msg);
}


