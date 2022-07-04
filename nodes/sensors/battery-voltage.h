#ifndef BATTERY_VOLTAGE_H
#define BATTERY_VOLTAGE_H

#define SAMPLING_INTERVAL         5
#define SAMPLE_LOWER_BOUND        116
#define SAMPLE_UPPER_BOUND        125

PROCESS_NAME(battery_voltage_sensor_process);

extern process_event_t BATTERY_VOLTAGE_SAMPLE_EVENT;
extern process_event_t BATTERY_VOLTAGE_EVENT_SUB;

#endif 