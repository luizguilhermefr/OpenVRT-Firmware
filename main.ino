#include <SoftwareSerial.h>
#include "openvrt.h"
#include "treadmill.h"

#define BAUD_RATE 9600

#define BTSerial Serial1

#define ANALOG_SPEED_PIN A8

#define ANALOG_MAX_SPEED 1000

#define SPEED_MAX_KM_H 40.0

#define DELAY 500

double current_rate;

double current_speed;

char current_measurement;

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
  openvrt_message_t *res = make_ack(msg->id, ACK_OP);
  send(res);
  free(res);
}

void refuse(openvrt_message_t *msg)
{
  openvrt_message_t *res = make_ack(msg->id, REFUSE_OP);
  send(res);
  free(res);
}

void print(openvrt_message_t *msg)
{
  char *message_as_str = message_as_string(msg);
  Serial.println(message_as_str);
  free(message_as_str);
}

bool next_rate(char data[DATA_LEN])
{
  char form[DATA_LEN + 2];
  for (int i = 0; i < DATA_LEN - 2; i++) { form[i] = data[i]; }
  form[DATA_LEN - 2] = '.';
  for (int i = DATA_LEN - 2; i < DATA_LEN; i++) { form[i + 1] = data[i]; }
  form[DATA_LEN + 2 - 1] = '\0';
  current_rate = strtod((char *) form, NULL);
}

bool next_measurement(char data[DATA_LEN])
{
  // TODO: Parse actual measurement
  current_measurement = MEASUREMENT_K_HA;

  return true;
}

bool take_action(openvrt_message_t *msg)
{
  switch (msg->opcode) {
    case RATE_SET:
      return next_rate(msg->data);
    case MEASURE_SET:
      return next_measurement(msg->data);
    case HANDSHAKE:
    case ACK_OP:
    case REFUSE_OP:
      return true;
  }
}

void next_speed()
{
  int sensorValue = min(analogRead(ANALOG_SPEED_PIN), ANALOG_MAX_SPEED);
  current_speed = (sensorValue / (double) ANALOG_MAX_SPEED) * SPEED_MAX_KM_H;
}

void setup()
{
  Serial.begin(BAUD_RATE);
  BTSerial.begin(BAUD_RATE);
  setup_treadmill();
  current_rate = 0.0;
  current_speed = 0.0;
  current_measurement = MEASUREMENT_K_HA;
}

void loop()
{
  if (available()) {
    openvrt_message_t *msg = receive();
    if (is_valid_message(msg)) {
      switch (msg->opcode) {
        case RATE_SET:
          next_rate(msg->data);
//        Serial.print("New rate: "); // TODO: Remove print
//        Serial.println(current_rate); // TODO: Remove print
          acknowledge(msg);
          break;
        case MEASURE_SET:
          next_measurement(msg->data) ? acknowledge(msg) : refuse(msg);
          break;
        case HANDSHAKE:
          acknowledge(msg);
      }
    } else {
      Serial.println("Invalid message..."); // TODO: Remove print
      refuse(msg);
    }
  }
  next_speed();
  next_tick(current_speed);
//Serial.print("New velocity: "); // TODO: Remove print
//Serial.println(current_velocity); // TODO: Remove print
  delay(DELAY);
}
