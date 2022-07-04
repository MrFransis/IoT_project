#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./energy-generated.h"
#include "./utils.h"
#include <stdio.h>

process_event_t ENERGY_SAMPLE_EVENT;
process_event_t ENERGY_EVENT_SUB;
struct process *subscriber;
int sample;

PROCESS(energy_sensor_process, "Energy sensor process");

PROCESS_THREAD(energy_sensor_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  ENERGY_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == ENERGY_EVENT_SUB);
  subscriber = (struct process *)data;

  etimer_set(&et, CLOCK_SECOND);
  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    sample = sensor_rand_int(SAMPLE_LOWER_BOUND, SAMPLE_UPPER_BOUND);
    process_post(subscriber, ENERGY_SAMPLE_EVENT, &sample);
    etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
