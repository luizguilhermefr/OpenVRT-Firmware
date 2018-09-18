#ifndef OPENVRT_FIRMWARE_TREADMILL_H
#define OPENVRT_FIRMWARE_TREADMILL_H

#include <AFMotor.h>

#define MEASUREMENT_K_HA 0xAA

#define MOTOR_PORT 4

#define MAX_SUPPORTED_SPEED_KM_H 40.0

#define MAX_TREADMILL_SPEED 255

AF_DCMotor motor(MOTOR_PORT);

void setup_treadmill()
{
  motor.setSpeed(0);
  motor.run(RELEASE);
}

void next_tick(double speed)
{
  int treadmill_speed = (min(speed, MAX_SUPPORTED_SPEED_KM_H) / MAX_SUPPORTED_SPEED_KM_H)
      * MAX_TREADMILL_SPEED;

  motor.run(FORWARD);

  //Begin example
  uint8_t i;

  for (i=0; i<255; i++) {
    motor.setSpeed(treadmill_speed);
    delay(10);
  }

  for (i=255; i!=0; i--) {
    motor.setSpeed(treadmill_speed);
    delay(10);
  }

  motor.run(BACKWARD);
  for (i=0; i<255; i++) {
    motor.setSpeed(treadmill_speed);
    delay(10);
  }

  for (i=255; i!=0; i--) {
    motor.setSpeed(treadmill_speed);
    delay(10);
  }


  motor.run(RELEASE);
  // End example
}

#endif //OPENVRT_FIRMWARE_TREADMILL_H
