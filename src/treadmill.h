#ifndef OPENVRT_FIRMWARE_TREADMILL_H
#define OPENVRT_FIRMWARE_TREADMILL_H

#include <float.h>

#include <AFMotor.h>

#include <PID_v1.h>

#define ACTUATOR_VERBOSE 1

#define MEASUREMENT_K_HA "KG_HA"

#define MOTOR_PORT 4

#define PROXIMITY_PORT 21

#define MAX_SUPPORTED_SPEED_M_S 10.0

#define MAX_PWM 255.0

#define MAX_RPM 1344.0

#define M2_PER_HA 10000.0

#define MAX_KG_S 1.0

#define PID_KP 500.0

#define PID_KI 200.0

#define PID_KD 50.0

static bool verbose;

static volatile unsigned long revolutions;

static unsigned long last_actuator_tick;

static unsigned long last_revolution_tick;

static unsigned long last_verbose_tick;

static unsigned long current_rpm;

static double current_speed_m_s;

static double desired_kg_ha;

static double desired_kg_m;

static double current_kg_m;

static double current_kg_s;

static double current_work_width_m;

static double current_pwm;

PID pid_controller(&current_kg_m, &current_pwm, &desired_kg_m, PID_KP, PID_KI, PID_KD, DIRECT);

AF_DCMotor motor(MOTOR_PORT);

static void calculate_revolutions_per_minute()
{
  current_rpm = revolutions * 6 * 2;
  revolutions = 0;
}

static void calculate_current_kg_per_second()
{
  current_kg_s = (min(current_rpm, MAX_RPM) / MAX_RPM) * MAX_KG_S;
}

static void calculate_current_kg_per_meter()
{
  if (current_kg_s == 0) {
    current_kg_m = 0.0;
  } else if (current_speed_m_s == 0) {
    current_kg_m = FLT_MAX;
  } else {
    current_kg_m = (current_kg_s * current_work_width_m) / current_speed_m_s;
  }
}

static void calculate_desired_kg_per_meter()
{
  desired_kg_m = current_speed_m_s == 0 ? 0.0 : desired_kg_ha / M2_PER_HA;
}

static void update_dc_pwm()
{
  calculate_current_kg_per_second();
  calculate_current_kg_per_meter();
  calculate_desired_kg_per_meter();

  if (current_speed_m_s > 0) {
    pid_controller.Compute();
    motor.setSpeed(current_pwm);
  } else {
    motor.setSpeed(0);
  }
}

bool supported_measurement(char *measurement)
{
  return strcmp(measurement, MEASUREMENT_K_HA) == 0x0;
}

void inc_revolution()
{
  revolutions++;
}

void actuator_set_speed(float speed_m_s)
{
  current_speed_m_s = speed_m_s;
}

void actuator_set_rate(float rate_kg_ha)
{
  desired_kg_ha = rate_kg_ha;
}

void actuator_set_work_width(float work_width_m)
{
  current_work_width_m = work_width_m;
}

void print_state()
{
  Serial.print(current_speed_m_s);
  Serial.print("\t");
  Serial.println(current_work_width_m);
  Serial.print("\t");
  Serial.print(desired_kg_ha);
  Serial.print("\t");
  Serial.print(desired_kg_m);
  Serial.print("\t");
  Serial.print(current_rpm);
  Serial.print("\t");
  Serial.print(current_kg_s);
  Serial.print("\t");
  Serial.print(current_kg_m);
  Serial.print("\t");
  Serial.print(current_kg_m - desired_kg_m);
  Serial.print("\t");
  Serial.println(current_pwm);
}

void actuator_setup()
{
  revolutions = current_rpm = current_pwm = 0;
  motor.setSpeed(current_pwm);
  motor.run(FORWARD);
  last_revolution_tick = last_actuator_tick = last_verbose_tick = millis();
  current_speed_m_s = desired_kg_ha = current_kg_m = current_kg_s = desired_kg_m = 0.0;
  current_work_width_m = 1.0;
  pinMode(PROXIMITY_PORT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PORT), inc_revolution, RISING);
  pid_controller.SetMode(AUTOMATIC);
}

void actuator_loop(unsigned long now_ms)
{
  if (now_ms - last_actuator_tick > 10) {
    last_actuator_tick = now_ms;
    update_dc_pwm();
  }

  if (now_ms - last_revolution_tick > 5000) {
    last_revolution_tick = now_ms;
    calculate_revolutions_per_minute();
  }

  if (ACTUATOR_VERBOSE && now_ms - last_verbose_tick > 5000) {
    last_verbose_tick = now_ms;
    print_state();
  }
}

#endif //OPENVRT_FIRMWARE_TREADMILL_H
