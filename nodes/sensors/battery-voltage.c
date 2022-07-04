#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./battery-voltage.h"
#include "./utils.h"
#include <stdio.h>

process_event_t BATTERY_VOLTAGE_SAMPLE_EVENT;
process_event_t BATTERY_VOLTAGE_EVENT_SUB;
struct process *subscriber;
int sample;

PROCESS(battery_voltage_sensor_process, "Battery voltage sensor process");

PROCESS_THREAD(battery_voltage_sensor_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  BATTERY_VOLTAGE_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == BATTERY_VOLTAGE_EVENT_SUB);
  subscriber = (struct process *)data;

  etimer_set(&et, CLOCK_SECOND*SAMPLING_INTERVAL);
  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    sample = sensor_rand_int(SAMPLE_LOWER_BOUND, SAMPLE_UPPER_BOUND);
    process_post(subscriber, BATTERY_VOLTAGE_SAMPLE_EVENT, &sample);
    etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
