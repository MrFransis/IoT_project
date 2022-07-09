#ifndef MOTOR_RPM_H_
#define MOTOR_RPM_H_

#define SAMPLING_INTERVAL         5
#define RPM_LOWER_BOUND        1500
#define RPM_UPPER_BOUND        1500

PROCESS_NAME(motor_rpm_sensor_process);

extern process_event_t MOTOR_RPM_SAMPLE_EVENT;
extern process_event_t MOTOR_RPM_EVENT_SUB;

#endif 