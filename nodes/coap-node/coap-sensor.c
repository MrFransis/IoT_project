#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os/dev/serial-line.h"
#include "contiki.h"
#include "coap-engine.h"
#include "os/sys/etimer.h"
#include "./resources/res-coolant.h"
#include "../sensors/coolant-level.h"

#define SENSOR_ID_LENGTH 10
#define COAP_MONITOR_STATE_STARTED 0
#define COAP_MONITOR_STATE_OPERATIONAL 1

#define SERVER_EP "coap://[fd00::1]:5683"
char *service_url = "coapRegistration";

PROCESS_NAME(coap_server);
AUTOSTART_PROCESSES(&coap_server);


static uint8_t state;
bool registered = false;

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t res_coolant; 

/*---------------------------------------------------------------------------*/
static void 
sensors_emulation(process_event_t event, int sample)
{
  //Engine temperature 
  if(event == COOLANT_SAMPLE_EVENT){
    res_coolant_update(sample);
  }
}

/*---------------------------------------------------------------------------*/
/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(coap_message_t *response)
{
  const uint8_t *chunk;

  if(response == NULL) {
    puts("Request timed out");
    return;
  }
	registered = true;
  int len = coap_get_payload(response, &chunk);

  printf("|%.*s", len, (char *)chunk);
}

PROCESS(coap_server, "Coap Server");

static void
init_monitor()
{
  state = COAP_MONITOR_STATE_STARTED;
  res_coolant_activate();
  process_start(&coolant_sensor_process, NULL);
  process_post(&coolant_sensor_process, COOLANT_EVENT_SUB, &coap_server);
}

PROCESS_THREAD(coap_server, ev, data)
{
  static coap_endpoint_t server_ep;
  PROCESS_BEGIN();
  init_monitor();

  //Registration to the collector
  coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
  coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
  coap_set_header_uri_path(request, service_url);
  COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);

  while(!registered){
    COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
  }
  
  while(true) {
    PROCESS_WAIT_EVENT();

    if(state == COAP_MONITOR_STATE_STARTED){
			//if(have_connectivity()==true)  
			state = COAP_MONITOR_STATE_OPERATIONAL;
    }

    if(ev == COOLANT_SAMPLE_EVENT && state == COAP_MONITOR_STATE_OPERATIONAL){
      sensors_emulation(ev, *((int *)data));
    }
  }                             

  PROCESS_END();
}
