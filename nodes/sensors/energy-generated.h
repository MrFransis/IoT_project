#ifndef ENERGY_GENERATED_H_
#define ENERGY_GENERATED_H_

#define ENERGY_SAMPLING_INTERVAL    10
#define ENERGY_LOWER_BOUND        1462 
#define ENERGY_UPPER_BOUND        1511

PROCESS_NAME(energy_sensor_process);

extern process_event_t ENERGY_SAMPLE_EVENT;
extern process_event_t ENERGY_EVENT_SUB;

#endif 