#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./coolant-temperature.h"
#include "./utils.h"
#include <stdio.h>

process_event_t COOLANT_TEMPERATURE_SAMPLE_EVENT;
process_event_t COOLANT_TEMPERATURE_EVENT_SUB;
struct process *subscriber;
int sample;

PROCESS(coolant_temperature_sensor_process, "Coolant temperature sensor process");

PROCESS_THREAD(coolant_temperature_sensor_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  COOLANT_TEMPERATURE_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == COOLANT_TEMPERATURE_EVENT_SUB);
  subscriber = (struct process *)data;

  etimer_set(&et, CLOCK_SECOND*SAMPLING_INTERVAL);
  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    sample = sensor_rand_int(SAMPLE_LOWER_BOUND, SAMPLE_UPPER_BOUND);
    process_post(subscriber, COOLANT_TEMPERATURE_SAMPLE_EVENT, &sample);
    etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
