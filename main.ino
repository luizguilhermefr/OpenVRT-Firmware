#include <SoftwareSerial.h>
#include "openvrt.h"
#include "mbroker.h"

#define BAUD_RATE 9600

#define HC05PIN 13
#define RXPIN 10
#define TXPIN 11

#define DELAY 100

SoftwareSerial BTSerial(RXPIN, TXPIN);

bool available()
{
  return BTSerial.available() >= MESSAGE_LEN;
}

openvrt_message_t *receive()
{
  char msg_buf[MESSAGE_LEN];
  for (int i = 0; i < MESSAGE_LEN; i++) {
    msg_buf[i] = BTSerial.read();
  }
  openvrt_message_t *msg = (openvrt_message_t *) msg_buf;

  return msg;
}

void send(openvrt_message_t *msg)
{
  char *buf = (char *) msg;
  for (int i = 0; i < MESSAGE_LEN; i++) {
    BTSerial.write(buf[i]);
  }
}

void acknowledge(openvrt_message_t *msg)
{
  openvrt_message_t res;
  strcpy(res.signature, SIGNATURE);
  res.opcode = ACK_OP;
  res.major_ver = MAJOR_V;
  res.minor_ver = MINOR_V;
  res.id = next_id();
  sprintf(res.data, "%08lu", msg->id);
  send(&res);
}

void refuse(openvrt_message_t *msg)
{

}

void print(openvrt_message_t *msg)
{
  char *message_as_str = message_as_string(msg);
  Serial.println(message_as_str);
  free(message_as_str);
}

void setup()
{
  pinMode(HC05PIN, OUTPUT);
  Serial.begin(BAUD_RATE);
  BTSerial.begin(BAUD_RATE);
}

void loop()
{
  if (available()) {
    openvrt_message_t *msg = receive();
    if (is_valid_message(msg)) {
      acknowledge(msg);
    } else {
      Serial.println("Invalid message.");
    }
  }
  delay(DELAY);
}
