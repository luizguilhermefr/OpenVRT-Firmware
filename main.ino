#include <SoftwareSerial.h>

#define BAUD_RATE 9600

#define HC05PIN 13
#define RXPIN 10
#define TXPIN 11

#define DELAY 500

SoftwareSerial btSerial(RXPIN, TXPIN);

void setup() {
    pinMode(HC05PIN, OUTPUT);
    Serial.begin(BAUD_RATE);
    // Bluetooth
    btSerial.begin(BAUD_RATE);
    connection = new BTConnection(btSerial);
}

void loop() {
    delay(DELAY);
}
