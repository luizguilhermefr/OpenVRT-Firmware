#ifndef OPENVRT_FIRMWARE_TREADMILL_H
#define OPENVRT_FIRMWARE_TREADMILL_H

#include <AFMotor.h>

#define MEASUREMENT_K_HA "KG_HA"

#define MOTOR_PORT 4

#define PROXIMITY_PORT 21

#define MAX_SUPPORTED_SPEED_M_S 10.0

#define MAX_TREADMILL_SPEED_PWM 255

AF_DCMotor motor(MOTOR_PORT);

static uint8_t current_speed_pwm;

static volatile unsigned long revolutions;

static unsigned long last_actuator_tick;

static unsigned long last_revolution_tick;

static float current_speed;

static float current_rate;

static unsigned long rpm;

static void calculate_revolutions_per_minute()
{
  rpm = revolutions * 6;
  revolutions = 0;
  if (SERIAL_VERBOSE) {
    Serial.print(rpm);
    Serial.println(" RPM");
  }
}

static void update_dc_pwm()
{
  uint8_t desired_speed_pwm = (min(current_speed, MAX_SUPPORTED_SPEED_M_S) / MAX_SUPPORTED_SPEED_M_S)
      * MAX_TREADMILL_SPEED_PWM;

  current_speed_pwm = desired_speed_pwm;

  motor.setSpeed(current_speed_pwm);
}

bool supported_measurement(char *measurement)
{
  return strcmp(measurement, MEASUREMENT_K_HA) == 0x0;
}

void inc_revolution()
{
  revolutions++;
}

void actuator_setup()
{
  current_speed_pwm = revolutions = rpm = 0;
  motor.setSpeed(current_speed_pwm);
  motor.run(FORWARD);
  last_revolution_tick = last_actuator_tick = millis();
  current_speed = current_rate = 0.0;
  pinMode(PROXIMITY_PORT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PORT), inc_revolution, RISING);
}

void actuator_set_speed(float speed)
{
  current_speed = speed;
}

void actuator_set_rate(float rate)
{
  current_rate = rate;
}

void actuator_loop(unsigned long now_ms)
{
  if (now_ms - last_actuator_tick > 100) {
    last_actuator_tick = now_ms;
    update_dc_pwm();
  }

  if (now_ms - last_revolution_tick > 10000) {
    last_revolution_tick = now_ms;
    calculate_revolutions_per_minute();
  }
}

#endif //OPENVRT_FIRMWARE_TREADMILL_H
