#ifndef COOLANT_TEMPERATURE_H_
#define COOLANT_TEMPERATURE_H_

#define SAMPLING_INTERVAL         5
#define SAMPLE_LOWER_BOUND        67
#define SAMPLE_UPPER_BOUND        70

PROCESS_NAME(coolant_temperature_sensor_process);

extern process_event_t COOLANT_TEMPERATURE_SAMPLE_EVENT;
extern process_event_t COOLANT_TEMPERATURE_EVENT_SUB;

#endif 