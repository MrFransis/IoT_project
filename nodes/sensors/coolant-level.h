#ifndef COOLANT_LEVEL_H_
#define COOLANT_LEVEL_H_

#define COOLANT_SAMPLING_INTERVAL         15
#define COOLANT_LEVEL_LOWER_BOUND        29
#define COOLANT_LEVEL_UPPER_BOUND        31

PROCESS_NAME(coolant_sensor_process);

extern process_event_t COOLANT_SAMPLE_EVENT;
extern process_event_t COOLANT_EVENT_SUB;
extern process_event_t COOLANT_EVENT_REFILL;
#endif