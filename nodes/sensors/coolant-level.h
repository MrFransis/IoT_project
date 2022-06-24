#ifndef COOLANT_LEVEL_H_
#define COOLANT_LEVEL_H_

#define COOLANT_SAMPLING_INTERVAL         5

PROCESS_NAME(coolant_sensor_process);

extern process_event_t COOLANT_SAMPLE_EVENT;
extern process_event_t COOLANT_EVENT_SUB;
#endif