//==============================================================================
//
//          tftp.h
//
//==============================================================================

#define SERVER_PORT 60006
#define TIMEOUT     10

#define MESSAGE_SIZE    516
#define DATA_SIZE       512
#define FILENAME_LENGTH 500
#define MODE_LENGTH     8
#define ERROR_LENGTH    512

#define STATE_READY         700
#define STATE_ACK_SENT      701
#define STATE_WAITING_ACK   702
#define STATE_WAITING_AC    701
#define STATE_WAITING_A     701

//================================================================================
// Packet
//================================================================================
typedef struct {
    unsigned short opcode;
    char message[516];
    
} Packet;

//================================================================================
// RRQ/WRQ Packet
//================================================================================
typedef struct {
    // Parent fields
    unsigned short opcode;
    char message[516];

    // RWRQ fields
    char filename[FILENAME_LENGTH];
    char mode[MODE_LENGTH];

} RWRQ_Packet;

//================================================================================
// DATA Packet
//================================================================================
typedef struct {
    // Parent fields
    unsigned short opcode;
    char message[516];

    // DATA fields
    unsigned short block_num;
    char data[DATA_SIZE];

} DATA_Packet;

//================================================================================
// ACK Packet
//================================================================================
typedef struct {
    // Parent fields
    unsigned short opcode;
    char message[516];

    // ACK fields
    unsigned short block_num;


} ACK_Packet;

//================================================================================
// ERROR Packet
//================================================================================
typedef struct {
    // Parent fields
    unsigned short opcode;
    char message[516];

    // ERROR fields
    unsigned short error_code;
    char error_message[ERROR_LENGTH];

} ERROR_Packet;

//================================================================================
// File_Container
//================================================================================
typedef struct {
    FILE * fp;
    int count;
    char current_data[DATA_SIZE];
} File_Container;

//================================================================================
// Functions
//================================================================================

// Packet
void Packet_init(Packet * thisP, unsigned short opcode);
void RWRQ_Packet_construct_msg(RWRQ_Packet * thisP, unsigned short opcode, char * message);
void RWRQ_Packet_construct(RWRQ_Packet * thisP, unsigned short opcode, char * fname, char * mode);
void DATA_Packet_construct_msg(DATA_Packet * thisP, unsigned short opcode, char * message);
void DATA_Packet_construct(DATA_Packet * thisP, unsigned short opcode, unsigned short b_num, char * data);
void ACK_Packet_construct_msg(ACK_Packet * thisP, unsigned short opcode, char * message);
void ACK_Packet_construct(ACK_Packet * thisP, unsigned short opcode, unsigned short b_num);
void ERROR_Packet_construct_msg(ERROR_Packet * thisP, unsigned short opcode, char * message);
void ERROR_Packet_construct(ERROR_Packet * thisP, unsigned short opcode, unsigned short e_code, char * error_msg);

// Message Parsing
unsigned short read_message_opcode(char * message);
unsigned short read_message_block_num(char * message);
unsigned short read_message_error_code(char * message);
char * read_message_filename(char * message);
char * read_message_mode(char * message);
char * read_message_data(char * message);
char * read_message_error_msg(char * message);

// FILE
File_Container * file_open(char * filename, char * op);
int file_read_next(File_Container * this_file);
int file_write_next(File_Container * this_file);
int file_get_size(File_Container * this_file);

// Utility
void write_debug(char * message);
void print_packet(Packet * packet);


