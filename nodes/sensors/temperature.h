#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#define TEMPERATURE_SAMPLING_INTERVAL         5

PROCESS_NAME(temperature_sensor_process);

extern process_event_t TEMPERATURE_SAMPLE_EVENT;
extern process_event_t TEMPERATURE_EVENT_SUB;

#endif 