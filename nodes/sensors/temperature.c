#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./temperature.h"
#include "./utils.h"
#include <stdio.h>

#define OFF 0
#define ON 1

process_event_t TEMPERATURE_SAMPLE_EVENT;
process_event_t TEMPERATURE_EVENT_SUB;
process_event_t TEMPERATURE_EVENT_ALERT;


PROCESS(temperature_sensor_process, "Temperature sensor process");

PROCESS_THREAD(temperature_sensor_process, ev, data)
{
  static struct etimer et;
  static struct process *subscriber;
  static int sample;
  static bool alert;
  PROCESS_BEGIN();

  printf("Temperature process started");

  alert = OFF;
  TEMPERATURE_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == TEMPERATURE_EVENT_SUB);

  subscriber = (struct process *)data;
  etimer_set(&et, CLOCK_SECOND*TEMPERATURE_SAMPLING_INTERVAL);
  while(true) {
    PROCESS_YIELD();
    if(etimer_expired(&et)){
      if(alert == OFF){
        sample = sensor_rand_int(TEMPERATURE_LOWER_BOUND, TEMPERATURE_UPPER_BOUND);
      }else if(alert == ON){
        sample -= 1;
      }
      process_post_synch(subscriber, TEMPERATURE_SAMPLE_EVENT, &sample);
      etimer_reset(&et);
    }else if(ev == TEMPERATURE_EVENT_ALERT){
      alert = data;
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
