#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#define SAMPLING_INTERVAL         5
#define TEMPERATURE_LOWER_BOUND        150
#define TEMPERATURE_UPPER_BOUND        160

PROCESS_NAME(temperature_sensor_process);

extern process_event_t TEMPERATURE_SAMPLE_EVENT;
extern process_event_t TEMPERATURE_EVENT_SUB;

#endif 