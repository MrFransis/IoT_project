#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./motor-rpm.h"
#include "./utils.h"
#include <stdio.h>

process_event_t MOTOR_RPM_SAMPLE_EVENT;
process_event_t MOTOR_RPM_EVENT_SUB;
struct process *subscriber;
int sample;

PROCESS(motor_rpm_sensor_process, "Motor RPM sensor process");

PROCESS_THREAD(motor_rpm_sensor_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  MOTOR_RPM_SAMPLE_EVENT = process_alloc_event();
  PROCESS_WAIT_EVENT_UNTIL(ev == MOTOR_RPM_EVENT_SUB);
  subscriber = (struct process *)data;

  etimer_set(&et, CLOCK_SECOND);
  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    sample = sensor_rand_int(RPM_LOWER_BOUND, RPM_UPPER_BOUND);
    process_post(subscriber, MOTOR_RPM_SAMPLE_EVENT, &sample);
    etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
