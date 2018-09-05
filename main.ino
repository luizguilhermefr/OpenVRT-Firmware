#include <SoftwareSerial.h>
#include "openvrt.h"

#define BAUD_RATE 9600

#define HC05PIN 13
#define RXPIN 10
#define TXPIN 11

#define DELAY 500

SoftwareSerial BTSerial(RXPIN, TXPIN);

bool response_available() {
    return BTSerial.available() >= MESSAGE_LEN;
}

openvrt_message_t *make_message_from_command() {
    char msg_buf[MESSAGE_LEN];
    for (int i = 0; i < MESSAGE_LEN; i++) {
        msg_buf[i] = BTSerial.read();
    }
    openvrt_message_t *msg = (openvrt_message_t *) msg_buf;

    return msg;
}

void setup() {
    pinMode(HC05PIN, OUTPUT);
    Serial.begin(BAUD_RATE);
    BTSerial.begin(BAUD_RATE);
}

void loop() {
    if (response_available()) {
        openvrt_message_t *msg = make_message_from_command();
        if (is_valid_message(msg)) {
            Serial.println("Valid.");
        } else {
            Serial.println("Invalid.");
        }
    }
    delay(DELAY);
}
