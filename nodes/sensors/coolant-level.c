#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./coolant-level.h"
#include "./utils.h"
#include <stdio.h>

process_event_t COOLANT_SAMPLE_EVENT;
process_event_t COOLANT_EVENT_SUB;
struct process *subscriber;
int sample;

PROCESS(coolant_sensor_process, "Coolant sensor process");

PROCESS_THREAD(coolant_sensor_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  COOLANT_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == COOLANT_EVENT_SUB);
  subscriber = (struct process *)data;

  etimer_set(&et, CLOCK_SECOND*SAMPLING_INTERVAL);
  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    sample = sensor_rand_int(COOLANT_LEVEL_LOWER_BOUND, COOLANT_LEVEL_UPPER_BOUND);
    process_post(subscriber, COOLANT_SAMPLE_EVENT, &sample);
    etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
