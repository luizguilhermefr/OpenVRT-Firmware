#ifndef OPENVRT_FIRMWARE_OPENVRT_H
#define OPENVRT_FIRMWARE_OPENVRT_H

#define MESSAGE_LEN 24
#define SIGNATURE_LEN 7
#define DATA_LEN 8

typedef struct {
    char signature[SIGNATURE_LEN];
    unsigned short major_ver;
    unsigned short minor_ver;
    unsigned int id;
    char opcode;
    char data[DATA_LEN];
} message;
#endif //OPENVRT_FIRMWARE_OPENVRT_H
