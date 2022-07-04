#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./temperature.h"
#include "./utils.h"
#include <stdio.h>

process_event_t TEMPERATURE_SAMPLE_EVENT;
process_event_t TEMPERATURE_EVENT_SUB;
struct process *subscriber;
int sample;

PROCESS(temperature_sensor_process, "Temperature sensor process");

PROCESS_THREAD(temperature_sensor_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  TEMPERATURE_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == TEMPERATURE_EVENT_SUB);
  subscriber = (struct process *)data;

  etimer_set(&et, CLOCK_SECOND*SAMPLING_INTERVAL);
  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    sample = sensor_rand_int(SAMPLE_LOWER_BOUND, SAMPLE_UPPER_BOUND);
    process_post(subscriber, TEMPERATURE_SAMPLE_EVENT, &sample);
    etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
