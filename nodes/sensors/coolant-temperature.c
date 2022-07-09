#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./coolant-temperature.h"
#include "./utils.h"
#include <stdio.h>

#define OFF 0
#define ON 1

process_event_t COOLANT_TEMPERATURE_SAMPLE_EVENT;
process_event_t COOLANT_TEMPERATURE_EVENT_SUB;
process_event_t COOLANT_TEMPERATURE_EVENT_ALERT;
struct process *subscriber;
int sample;
int alert;

PROCESS(coolant_temperature_sensor_process, "Coolant temperature sensor process");

PROCESS_THREAD(coolant_temperature_sensor_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  alert = OFF;
  COOLANT_TEMPERATURE_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == COOLANT_TEMPERATURE_EVENT_SUB);
  subscriber = (struct process *)data;

  etimer_set(&et, CLOCK_SECOND*SAMPLING_INTERVAL);
  while(true) {
    PROCESS_YIELD();
    if(ev == etimer_expired(&et)){
      if(alert == OFF){
        sample = sensor_rand_int(COOLANT_TEMPERATURE_LOWER_BOUND, COOLANT_TEMPERATURE_UPPER_BOUND);
      }else if(alert == ON){
        sample -= 1;
      }
    process_post(subscriber, COOLANT_TEMPERATURE_SAMPLE_EVENT, &sample);
      etimer_reset(&et);
    }else if(ev == COOLANT_TEMPERATURE_EVENT_ALERT){
      alert = *((int *)data);
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
