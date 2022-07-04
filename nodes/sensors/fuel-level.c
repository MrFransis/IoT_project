#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./fuel-level.h"
#include "./utils.h"
#include <stdio.h>

process_event_t FUEL_LEVEL_SAMPLE_EVENT;
process_event_t FUEL_LEVEL_EVENT_SUB;
struct process *subscriber;
int sample;

PROCESS(fuel_level_sensor_process, "Fuel level sensor process");

PROCESS_THREAD(fuel_level_sensor_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  FUEL_LEVEL_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == FUEL_LEVEL_EVENT_SUB);
  subscriber = (struct process *)data;

  etimer_set(&et, CLOCK_SECOND);
  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    sample = sensor_rand_int(SAMPLE_LOWER_BOUND, SAMPLE_UPPER_BOUND);
    process_post(subscriber, FUEL_LEVEL_SAMPLE_EVENT, &sample);
    etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
