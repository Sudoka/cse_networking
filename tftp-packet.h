#define SERVER_PORT 6006

#define MESSAGE_SIZE    516
#define DATA_SIZE       512
#define FILENAME_LENGTH 500
#define MODE_LENGTH     8
#define ERROR_LENGTH    512


// Packet
typedef struct {
    unsigned short opcode;
    char message[516];
    
} Packet;

// RRQ/WRQ Packet
typedef struct {
    // Parent fields
    unsigned short opcode;
    char message[516];

    // RWRQ fields
    char filename[FILENAME_LENGTH];
    char mode[MODE_LENGTH];

} RWRQ_Packet;

// DATA Packet
typedef struct {
    // Parent fields
    unsigned short opcode;
    char message[516];

    // DATA fields
    unsigned short block_num;
    char data[DATA_SIZE];

} DATA_Packet;

// ACK Packet
typedef struct {
    // Parent fields
    unsigned short opcode;
    char message[516];

    // ACK fields
    unsigned short block_num;


} ACK_Packet;

// ERROR Packet
typedef struct {
    // Parent fields
    unsigned short opcode;
    char message[516];

    // ERROR fields
    unsigned short error_code;
    char error_message[ERROR_LENGTH];

} ERROR_Packet;

