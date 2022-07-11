#ifndef FUEL_LEVEL_H_
#define FUEL_LEVEL_H_

#define FUEL_SAMPLING_INTERVAL  10
#define FUEL_LOWER_BOUND        28
#define FUEL_UPPER_BOUND        28

PROCESS_NAME(fuel_level_sensor_process);

extern process_event_t FUEL_LEVEL_SAMPLE_EVENT;
extern process_event_t FUEL_LEVEL_EVENT_SUB;
extern process_event_t FUEL_LEVEL_EVENT_REFILL;

#endif 