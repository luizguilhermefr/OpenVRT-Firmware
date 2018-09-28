#ifndef OPENVRT_FIRMWARE_OPENVRT_H
#define OPENVRT_FIRMWARE_OPENVRT_H

#include "util.h"
#include "mbroker.h"

#define MESSAGE_LEN 24
#define SIGNATURE_LEN 7
#define DATA_LEN 8

#define SIGNATURE "OPENVRT"
#define MAJOR_V 1
#define MINOR_V 0

#define REFUSE_OP 0x0
#define ACK_OP 0x1
#define HANDSHAKE 0x2
#define RATE_SET 0x3
#define MEASURE_SET 0x4

typedef struct message
{
  char signature[SIGNATURE_LEN];
  uint16_t major_ver;
  uint16_t minor_ver;
  uint32_t id;
  char opcode;
  char data[DATA_LEN];
} __attribute__((packed)) openvrt_message_t;

bool is_valid_signature(char signature[SIGNATURE_LEN])
{
  return strncmp(SIGNATURE, signature, SIGNATURE_LEN) == 0x0;
}

bool is_valid_version(uint16_t major)
{
  return major == MAJOR_V;
}

bool is_valid_opcode(char opcode)
{
  switch (opcode) {
    case REFUSE_OP:
    case ACK_OP:
    case HANDSHAKE:
    case RATE_SET:
    case MEASURE_SET:
      return true;
    default:
      return false;
  }
}

bool is_valid_message(openvrt_message_t *msg)
{
  if (!is_valid_signature(msg->signature)) {
    return false;
  }

  if (!is_valid_version(msg->major_ver)) {
    return false;
  }

  if (!is_valid_opcode(msg->opcode)) {
    return false;
  }

  return true;
}

char const *opcode_as_string(char opcode)
{
  switch (opcode) {
    case REFUSE_OP:
      return "REFUSE";
    case ACK_OP:
      return "ACK";
    case HANDSHAKE:
      return "HANDSHAKE";
    case RATE_SET:
      return "RATE_SET";
    case MEASURE_SET:
      return "MEASURE_SET";
    default:
      return "";
  }
}

char *message_as_string(openvrt_message_t *msg)
{
  char *buf = (char *) malloc(sizeof(char) * 255);
  char *signature = make_null_terminated_string(msg->signature, SIGNATURE_LEN);
  char *data = make_null_terminated_string(msg->data, DATA_LEN);
  const char *opcode_str = opcode_as_string(msg->opcode);
  sprintf(
      buf,
      "signature: %s\nversion: %hu.%hu\nid: %lu\nopcode: %s (0x%02x)\ndata: {%s}\n",
      signature,
      msg->major_ver,
      msg->minor_ver,
      msg->id,
      opcode_str,
      msg->opcode,
      data
  );
  return buf;
}

openvrt_message_t *make_ack(uint32_t target_id, char opcode = ACK_OP)
{
  openvrt_message_t *res = (openvrt_message_t *) malloc(sizeof(openvrt_message_t));
  strcpy(res->signature, SIGNATURE);
  res->opcode = opcode == ACK_OP ? ACK_OP : REFUSE_OP;
  res->major_ver = MAJOR_V;
  res->minor_ver = MINOR_V;
  res->id = next_id();
  sprintf(res->data, "%08lu", target_id);
  return res;
}

#endif //OPENVRT_FIRMWARE_OPENVRT_H
