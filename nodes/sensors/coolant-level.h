#ifndef COOLANT_LEVEL_H_
#define COOLANT_LEVEL_H_

#define SAMPLING_INTERVAL         5
#define SAMPLE_LOWER_BOUND        30
#define SAMPLE_UPPER_BOUND        30

PROCESS_NAME(coolant_sensor_process);

extern process_event_t COOLANT_SAMPLE_EVENT;
extern process_event_t COOLANT_EVENT_SUB;
#endif