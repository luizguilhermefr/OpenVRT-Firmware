#include <SoftwareSerial.h>
#include "openvrt.h"
#include "treadmill.h"

#define BAUD_RATE 9600

#define BTSerial Serial1

#define ANALOG_SPEED_PIN A8

#define ANALOG_MAX_SPEED 1000.0

#define SPEED_MAX_M_S 10.0

#define PROTOCOL_VERBOSE 0

#define SPEED_SENSOR_VCOUNT 10

unsigned long speed_sensor_vi;

unsigned long now;

unsigned long last_message_tick;

unsigned long last_speed_tick;

char *current_measurement;

bool available()
{
  bool is_available = BTSerial.available() >= MESSAGE_LEN;
  if (is_available && PROTOCOL_VERBOSE) Serial.println("MESSAGE RECEIVED.");
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
  if (PROTOCOL_VERBOSE) Serial.println("EMITTING ACKNOWLEDGEMENT.");
  openvrt_message_t *res = make_ack(msg->id, ACK_OP);
  send(res);
  free(res);
}

void refuse(openvrt_message_t *msg)
{
  if (PROTOCOL_VERBOSE) Serial.println("EMITTING REFUSAL.");
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
  float r = strtod((char *) form, NULL);
  if (r < 0) {
    return false;
  }
  actuator_set_rate(r);
  if (PROTOCOL_VERBOSE) {
    Serial.print("NEW RATE IS ");
    Serial.println(r);
  }
  return true;
}

void next_work_width(char data[DATA_LEN])
{
  char form[DATA_LEN + 2];
  for (int i = 0; i < DATA_LEN - 2; i++) { form[i] = data[i]; }
  form[DATA_LEN - 2] = '.';
  for (int i = DATA_LEN - 2; i < DATA_LEN; i++) { form[i + 1] = data[i]; }
  form[DATA_LEN + 2 - 1] = '\0';
  float ww = strtod((char *) form, NULL);
  if (ww <= 0) {
    return false;
  }
  actuator_set_work_width(ww);
  if (PROTOCOL_VERBOSE) {
    Serial.print("NEW WORK WIDTH IS ");
    Serial.println(ww);
  }
  return true;
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
    if (PROTOCOL_VERBOSE) {
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
  float s = (sensorValue / ANALOG_MAX_SPEED) * SPEED_MAX_M_S;
  actuator_set_speed(s);
}

void next_message()
{
  if (available()) {
    openvrt_message_t *msg = receive();
    if (is_valid_message(msg)) {
      switch (msg->opcode) {
        case RATE_SET:
          next_rate(msg->data) ? acknowledge(msg) : refuse(msg);
          break;
        case MEASURE_SET:
          next_measurement(msg->data) ? acknowledge(msg) : refuse(msg);
          break;
        case WORK_WIDTH_SET:
          next_work_width(msg->data) ? acknowledge(msg) : refuse(msg);
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
  current_measurement = (char *) malloc(sizeof(char) * (DATA_LEN + 1));
  strcpy(current_measurement, MEASUREMENT_K_HA);
  last_speed_tick = last_message_tick = millis();
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

  actuator_loop(now);
}
