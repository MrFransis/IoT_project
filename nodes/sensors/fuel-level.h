#ifndef FUEL_LEVEL_H_
#define FUEL_LEVEL_H_

#define SAMPLING_INTERVAL         5
#define FUEL_LOWER_BOUND        800
#define FUEL_UPPER_BOUND        800

PROCESS_NAME(fuel_level_sensor_process);

extern process_event_t FUEL_LEVEL_SAMPLE_EVENT;
extern process_event_t FUEL_LEVEL_EVENT_SUB;

#endif 