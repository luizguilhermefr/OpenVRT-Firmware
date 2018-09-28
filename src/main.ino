#include <SoftwareSerial.h>
#include "openvrt.h"
#include "treadmill.h"

#define BAUD_RATE 9600

#define BTSerial Serial1

#define ANALOG_SPEED_PIN A8

#define ANALOG_MAX_SPEED 1000

#define SPEED_MAX_KM_H 40.0

#define SERIAL_VERBOSE 1

unsigned long now;

unsigned long last_message_tick;

unsigned long last_speed_tick;

unsigned long last_actuator_tick;

double current_rate;

double current_speed;

char *current_measurement;

bool available()
{
  bool is_available = BTSerial.available() >= MESSAGE_LEN;
  if (is_available && SERIAL_VERBOSE) Serial.println("MESSAGE RECEIVED.");
  return is_available;
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
  if (SERIAL_VERBOSE) Serial.println("EMITTING ACKNOWLEDGEMENT.");
  openvrt_message_t *res = make_ack(msg->id, ACK_OP);
  send(res);
  free(res);
}

void refuse(openvrt_message_t *msg)
{
  if (SERIAL_VERBOSE) Serial.println("EMITTING REFUSAL.");
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

void next_rate(char data[DATA_LEN])
{
  char form[DATA_LEN + 2];
  for (int i = 0; i < DATA_LEN - 2; i++) { form[i] = data[i]; }
  form[DATA_LEN - 2] = '.';
  for (int i = DATA_LEN - 2; i < DATA_LEN; i++) { form[i + 1] = data[i]; }
  form[DATA_LEN + 2 - 1] = '\0';
  current_rate = strtod((char *) form, NULL);
  if (SERIAL_VERBOSE) {
    Serial.print("NEW RATE IS ");
    Serial.println(current_rate);
  }
}

bool next_measurement(char data[DATA_LEN])
{
  char *desired_measurement = (char *) malloc(sizeof(char) * (DATA_LEN + 1));
  uint8_t i = 0, n = 0;
  while (data[i] == '\0') i++;
  for (; i < DATA_LEN; i++, n++) {
    desired_measurement[n] = data[i];
  }
  desired_measurement[n] = '\0';

  if (supported_measurement(desired_measurement)) {
    current_measurement = desired_measurement;
    if (SERIAL_VERBOSE) {
      Serial.print("NEW MEASUREMENT IS ");
      Serial.println(current_measurement);
    }
    return true;
  }

  return false;
}

void next_speed()
{
  int sensorValue = min(analogRead(ANALOG_SPEED_PIN), ANALOG_MAX_SPEED);
  current_speed = (sensorValue / (double) ANALOG_MAX_SPEED) * SPEED_MAX_KM_H;
}

void next_message()
{
  if (available()) {
    openvrt_message_t *msg = receive();
    if (is_valid_message(msg)) {
      switch (msg->opcode) {
        case RATE_SET:
          next_rate(msg->data);
          acknowledge(msg);
          break;
        case MEASURE_SET:
          next_measurement(msg->data) ? acknowledge(msg) : refuse(msg);
          break;
        case HANDSHAKE:
          acknowledge(msg);
      }
    } else {
      refuse(msg);
    }
  }
}

void setup()
{
  Serial.begin(BAUD_RATE);
  BTSerial.begin(BAUD_RATE);
  actuator_setup();
  current_rate = 0.0;
  current_speed = 0.0;
  current_measurement = (char *) malloc(sizeof(char) * (DATA_LEN + 1));
  strcpy(current_measurement, MEASUREMENT_K_HA);
}

void loop()
{
  now = millis();

  if (now - last_message_tick > 100) {
    last_message_tick = now;
    next_message();
  }

  if (now - last_speed_tick > 500) {
    last_speed_tick = now;
    next_speed();
  }

  if (now - last_actuator_tick > 5) {
    last_actuator_tick = now;
    actuator_loop(current_speed);
  }
}
