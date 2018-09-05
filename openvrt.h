#ifndef OPENVRT_FIRMWARE_OPENVRT_H
#define OPENVRT_FIRMWARE_OPENVRT_H

#define MESSAGE_LEN 24
#define SIGNATURE_LEN 7
#define DATA_LEN 8

#define SIGNATURE "OPENVRT"
#define MAJOR_V = 0x0001
#define MINOR_V = 0x0000

typedef struct message {
    char signature[SIGNATURE_LEN];
    unsigned short major_ver;
    unsigned short minor_ver;
    unsigned int id;
    char opcode;
    char data[DATA_LEN];
} __attribute__((packed)) openvrt_message_t;

bool is_valid_signature(char signature[SIGNATURE_LEN]) {
    static const char *valid_sign = SIGNATURE;
    return strcmp(valid_sign, signature) == 0x00;
}

bool is_valid_message(openvrt_message_t *msg) {
    if (!is_valid_signature(msg->signature)) {
        return false;
    }

    return true;
}

#endif //OPENVRT_FIRMWARE_OPENVRT_H
