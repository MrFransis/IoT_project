#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./energy-generated.h"
#include "./utils.h"
#include <stdio.h>

process_event_t ENERGY_SAMPLE_EVENT;
process_event_t ENERGY_EVENT_SUB;

PROCESS(energy_sensor_process, "Energy sensor process");

PROCESS_THREAD(energy_sensor_process, ev, data)
{
  static struct process *subscriber;
  static int sample;
  static struct etimer et;
  static struct etimer etm;

  PROCESS_BEGIN();
  printf("Energy generator process started\n");
  etimer_set(&etm, 6*CLOCK_SECOND);
  ENERGY_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == ENERGY_EVENT_SUB);
  subscriber = (struct process *)data;
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&etm));
  etimer_set(&et, CLOCK_SECOND*ENERGY_SAMPLING_INTERVAL);
  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    sample = sensor_rand_int(ENERGY_LOWER_BOUND, ENERGY_UPPER_BOUND);
    process_post_synch(subscriber, ENERGY_SAMPLE_EVENT, &sample);
    etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
