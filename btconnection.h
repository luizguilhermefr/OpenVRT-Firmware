#ifndef OPENVRT_FIRMWARE_CONNECTION_H
#define OPENVRT_FIRMWARE_CONNECTION_H

#include <SoftwareSerial.h>

class BTConnection {
    SoftwareSerial softwareSerial;
public:
    BTConnection(SoftwareSerial);
};

BTConnection::BTConnection(SoftwareSerial serial) {
    softwareSerial = serial;
}

#endif //OPENVRT_FIRMWARE_CONNECTION_H
