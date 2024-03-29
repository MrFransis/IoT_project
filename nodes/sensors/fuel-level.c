#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./fuel-level.h"
#include "./utils.h"
#include <stdio.h>

#define LOG_MODULE "sensor"
#define LOG_LEVEL LOG_LEVEL_APP

process_event_t FUEL_LEVEL_SAMPLE_EVENT;
process_event_t FUEL_LEVEL_EVENT_SUB;
process_event_t FUEL_LEVEL_EVENT_REFILL;

PROCESS(fuel_level_sensor_process, "Fuel level sensor process");

PROCESS_THREAD(fuel_level_sensor_process, ev, data)
{
  static struct etimer et;
  static struct etimer etm;
  static struct process *subscriber;
  static int sample;
  PROCESS_BEGIN();

  LOG_INFO("Fuel level process started\n");
  
  etimer_set(&etm, 3*CLOCK_SECOND);
  sample = sensor_rand_int(FUEL_LOWER_BOUND, FUEL_UPPER_BOUND);
  FUEL_LEVEL_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == FUEL_LEVEL_EVENT_SUB);
  subscriber = (struct process *)data;
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&etm));
  etimer_set(&et, CLOCK_SECOND*FUEL_SAMPLING_INTERVAL);
  while(true){
    PROCESS_YIELD();
    if(etimer_expired(&et)){
      if(sample > 51){
        sample -= 50;
      }
      process_post(subscriber, FUEL_LEVEL_SAMPLE_EVENT, &sample);
      etimer_reset(&et);
    }else if(ev == FUEL_LEVEL_EVENT_REFILL){
      sample = sensor_rand_int(FUEL_LOWER_BOUND, FUEL_UPPER_BOUND);
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
