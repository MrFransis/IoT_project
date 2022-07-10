#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#define TEMPERATURE_SAMPLING_INTERVAL   30
#define TEMPERATURE_LOWER_BOUND        150
#define TEMPERATURE_UPPER_BOUND        160

PROCESS_NAME(temperature_sensor_process);

extern process_event_t TEMPERATURE_SAMPLE_EVENT;
extern process_event_t TEMPERATURE_EVENT_SUB;
extern process_event_t TEMPERATURE_EVENT_ALERT;

#endif 