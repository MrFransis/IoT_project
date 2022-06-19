#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os/dev/serial-line.h"
#include "contiki.h"
#include "coap-engine.h"
#include "os/sys/etimer.h"

/* Log configuration */
#include "sys/log.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
#define SENSOR_ID_LENGTH 10
#define COAP_MONITOR_STATE_STARTED 0
#define COAP_MONITOR_STATE_OPERATIONAL 1

struct coap_sensor {
  char sensor_id[SENSOR_ID_LENGTH];
  uint8_t state;
};

static struct coap_sensor machine;
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the reception of a new machine ID on the serial line.
 *
 *          The function handles the reception of a new machine ID
 *          on the serial line, updating the relative resource
 *          and restarting the sampling activity
 *          of the sensor processes. It changes the
 *          monitor state to COAP_MONITOR_STATE_OPERATIONAL.
 *//*
static void
handle_new_machine_ID(char *machine_id)
{
  memcpy(machine.sensor_id, machine_id, SENSOR_ID_LENGTH);
  machine.sensor_id[SENSOR_ID_LENGTH - 1] = '\0';
  LOG_INFO("New machine ID: %s.\n", machine.sensor_id);


  machine.state = COAP_MONITOR_STATE_OPERATIONAL;
}*/
/*---------------------------------------------------------------------------*/

static void
init_monitor()
{
  machine.state = COAP_MONITOR_STATE_STARTED;
}
/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t  res_obs; 

static struct etimer e_timer;

PROCESS(er_example_server, "Erbium Example Server");
AUTOSTART_PROCESSES(&er_example_server);

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();
  init_monitor();
  PROCESS_PAUSE();

  LOG_INFO("Starting Erbium Example Server\n");

  coap_activate_resource(&res_obs, "obs");
  
  etimer_set(&e_timer, CLOCK_SECOND * 4);
  
  printf("Loop\n");

  while(1) {
    PROCESS_WAIT_EVENT();
	/*
    if(ev == serial_line_event_message && machine.state == COAP_MONITOR_STATE_STARTED) {
      handle_new_machine_ID((char*)data);
      printf("Received ID\n");
      printf("%s\n", (char*)data);
      etimer_set(&e_timer, CLOCK_SECOND * 4);
    }*/

    if(ev == PROCESS_EVENT_TIMER && data == &e_timer && machine.state == COAP_MONITOR_STATE_OPERATIONAL){
      printf("Event triggered\n");
      
      res_obs.trigger();
      
      etimer_set(&e_timer, CLOCK_SECOND * 4);
    }
  }                             

  PROCESS_END();
}
