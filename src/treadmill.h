#ifndef OPENVRT_FIRMWARE_TREADMILL_H
#define OPENVRT_FIRMWARE_TREADMILL_H

#include <AFMotor.h>

#include <PID_v1.h>

#define MEASUREMENT_K_HA "KG_HA"

#define MOTOR_PORT 4

#define PROXIMITY_PORT 21

#define MAX_SUPPORTED_SPEED_M_S 10.0

#define MIN_PWM 0.0

#define MAX_PWM 255.0

#define MAX_RPM 1344

#define M2_PER_HA 10000.0

#define MAX_KG_S 5.0

#define PID_KP 1.0

#define PID_KI 0.1

#define PID_KD 0.5

AF_DCMotor motor(MOTOR_PORT);

static bool verbose;

static volatile unsigned long revolutions;

static unsigned long last_actuator_tick;

static unsigned long last_revolution_tick;

static unsigned long current_rpm;

static double current_speed_m_s;

static double desired_kg_ha;

static double desired_kg_m;

static double current_kg_m;

static double current_kg_s;

static double pwm;

PID pid_controller(&current_kg_m, &pwm, &desired_kg_m, PID_KP, PID_KI, PID_KD, DIRECT);

static void calculate_revolutions_per_minute()
{
  current_rpm = revolutions * 6;
  revolutions = 0;
  if (verbose) {
    Serial.print(current_rpm);
    Serial.println(" RPM");
  }
}

static void calculate_desired_kg_per_meter()
{
  desired_kg_m = desired_kg_ha / M2_PER_HA;
}

static void calculate_current_kg_per_second()
{
  current_kg_s = (min(current_rpm, MAX_RPM) / MAX_RPM) * MAX_KG_S;
}

static void calculate_current_kg_per_meter()
{
  current_kg_m = current_speed_m_s == 0 ? 0.0 : current_kg_s / current_speed_m_s;
}

static void update_dc_pwm()
{
  calculate_current_kg_per_second();
  calculate_current_kg_per_meter();
  calculate_desired_kg_per_meter();
  pid_controller.Compute();
  motor.setSpeed(pwm);
}

bool supported_measurement(char *measurement)
{
  return strcmp(measurement, MEASUREMENT_K_HA) == 0x0;
}

void inc_revolution()
{
  revolutions++;
}

void actuator_setup(bool _verbose)
{
  verbose = _verbose;
  pwm = MIN_PWM;
  revolutions = current_rpm = 0;
  motor.setSpeed(pwm);
  motor.run(FORWARD);
  last_revolution_tick = last_actuator_tick = millis();
  current_speed_m_s = desired_kg_ha = current_kg_m = current_kg_s = desired_kg_m = 0.0;
  pinMode(PROXIMITY_PORT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PORT), inc_revolution, RISING);
  pid_controller.SetMode(AUTOMATIC);
}

void actuator_set_speed(float speed_m_s)
{
  current_speed_m_s = speed_m_s;
}

void actuator_set_rate(float rate_kg_ha)
{
  desired_kg_ha = rate_kg_ha;
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
