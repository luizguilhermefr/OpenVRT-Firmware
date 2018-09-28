#ifndef OPENVRT_FIRMWARE_TREADMILL_H
#define OPENVRT_FIRMWARE_TREADMILL_H

#include <AFMotor.h>

#define MEASUREMENT_K_HA "KG_HA"

#define MOTOR_PORT 4

#define MAX_SUPPORTED_SPEED_KM_H 40.0

#define MAX_TREADMILL_SPEED_PWM 255

AF_DCMotor motor(MOTOR_PORT);

static uint8_t current_speed_pwm;

bool supported_measurement(char *measurement)
{
  return strcmp(measurement, MEASUREMENT_K_HA) == 0x0;
}

void actuator_setup()
{
  motor.setSpeed(0);
  motor.run(FORWARD);
  current_speed_pwm = 0;
}

void actuator_loop(double speed)
{
  uint8_t desired_speed_pwm = (min(speed, MAX_SUPPORTED_SPEED_KM_H) / MAX_SUPPORTED_SPEED_KM_H)
      * MAX_TREADMILL_SPEED_PWM;

  if (current_speed_pwm < desired_speed_pwm) {
    current_speed_pwm++;
  } else if (current_speed_pwm > desired_speed_pwm) {
    current_speed_pwm--;
  }

  motor.setSpeed(current_speed_pwm);
}

#endif //OPENVRT_FIRMWARE_TREADMILL_H
