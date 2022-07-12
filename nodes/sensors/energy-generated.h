#ifndef ENERGY_GENERATED_H_
#define ENERGY_GENERATED_H_

#define ENERGY_SAMPLING_INTERVAL    10
#define ENERGY_UPPER_BOUND        286062
#define ENERGY_LOWER_BOUND        285989
#define ENERGY_ALERT_UPPER_BOUND        206062
#define ENERGY_ALERT_LOWER_BOUND        195989

PROCESS_NAME(energy_sensor_process);

extern process_event_t ENERGY_SAMPLE_EVENT;
extern process_event_t ENERGY_EVENT_SUB;
extern process_event_t ENERGY_EVENT_ALERT;

#endif 