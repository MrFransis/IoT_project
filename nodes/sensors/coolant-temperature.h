#ifndef COOLANT_TEMPERATURE_H_
#define COOLANT_TEMPERATURE_H_

#define COOLANT_TEMP_SAMPLING_INTERVAL                      120
#define COOLANT_TEMPERATURE_LOWER_BOUND        67
#define COOLANT_TEMPERATURE_UPPER_BOUND        70

PROCESS_NAME(coolant_temperature_sensor_process);

extern process_event_t COOLANT_TEMPERATURE_SAMPLE_EVENT;
extern process_event_t COOLANT_TEMPERATURE_EVENT_SUB;
extern process_event_t COOLANT_TEMPERATURE_EVENT_ALERT;

#endif 