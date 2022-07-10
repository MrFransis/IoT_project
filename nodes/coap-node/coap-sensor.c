#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os/dev/serial-line.h"
#include "contiki.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "os/sys/etimer.h"
#include "os/net/ipv6/uip-ds6.h"
#include "dev/etc/rgb-led/rgb-led.h"
#include "../sensors/energy-generated.h"
#include "../sensors/fuel-level.h"
#include "../sensors/temperature.h"
#include "./resources/res-energy-generated.h"
#include "./resources/res-fuel-level.h"
#include "./resources/res-temperature.h"

#define SENSOR_ID_LENGTH 10
#define STATE_INIT    		  0
#define COAP_MONITOR_STATE_STARTED 1
#define COAP_MONITOR_STATE_OPERATIONAL 2

#define SERVER "coap://[fd00::1]:5683"

PROCESS_NAME(coap_server);
AUTOSTART_PROCESSES(&coap_server);

char *service_url = "coapReg";
static uint8_t state;
bool registered = false;
static int node_id;

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t res_coolant; 

/*---------------------------------------------------------------------------*/
static void 
sensors_emulation(process_event_t event, int sample)
{
  if (event == TEMPERATURE_SAMPLE_EVENT){
    printf("New temperature measurement with value %d. Updating collector.\n", sample);
    res_temperature_update(sample, node_id);
  } else if (event == FUEL_LEVEL_SAMPLE_EVENT) {
    printf("New fuel level measurement with value %d. Updating collector.\n", sample);
    res_fuel_level_update(sample, node_id);
  } else if (event == ENERGY_SAMPLE_EVENT) {
    printf("New energy generated measurement with value %d. Updating collector.\n", sample);
    res_energy_generated_update(sample, node_id);
  }
}

/*---------------------------------------------------------------------------*/
/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(coap_message_t *response)
{
  const uint8_t *chunk;
  if(response == NULL) {
    return;
  }
	state = COAP_MONITOR_STATE_OPERATIONAL;
  int len = coap_get_payload(response, &chunk);
  printf("|%.*s", len, (char *)chunk);
}

static bool
have_connectivity(void)
{
  if(uip_ds6_get_global(ADDR_PREFERRED) == NULL ||
     uip_ds6_defrt_choose() == NULL) {
    return false;
  }
  return true;
}

PROCESS(coap_server, "Coap Server");

static void
init_monitor()
{
  state = STATE_INIT;

  res_temperature_activate();
  process_start(&temperature_sensor_process, NULL);
  process_post(&temperature_sensor_process, TEMPERATURE_EVENT_SUB, &coap_server);

  res_fuel_level_activate();
  process_start(&fuel_level_sensor_process, NULL);
  process_post(&fuel_level_sensor_process, FUEL_LEVEL_EVENT_SUB, &coap_server);

  res_energy_generated_activate();
  process_start(&energy_sensor_process, NULL);
  process_post(&energy_sensor_process, ENERGY_EVENT_SUB, &coap_server);

}

static bool
sensor_event(process_event_t event)
{
  if(event == TEMPERATURE_SAMPLE_EVENT
      || event == FUEL_LEVEL_SAMPLE_EVENT
      || event == ENERGY_SAMPLE_EVENT) {
    return true;
  }
  return false;
}

PROCESS_THREAD(coap_server, ev, data)
{
  static coap_endpoint_t server;
  static coap_message_t request[1]; 
  PROCESS_BEGIN();
  init_monitor();

  node_id = linkaddr_node_addr.u8[7];

  //Registration to the collector
  coap_endpoint_parse(SERVER, strlen(SERVER), &server);
  coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
  coap_set_header_uri_path(request, service_url);
  
  while(true) {
    PROCESS_WAIT_EVENT();

    if(state==STATE_INIT){
      if(have_connectivity()==true)  
        state = COAP_MONITOR_STATE_STARTED;
    } 

    if(state == COAP_MONITOR_STATE_STARTED){
			COAP_BLOCKING_REQUEST(&server, request, client_chunk_handler);  
    }

    if(sensor_event(ev) && state == COAP_MONITOR_STATE_OPERATIONAL){
      sensors_emulation(ev, *((int *)data));
    }
  }                             

  PROCESS_END();
}
