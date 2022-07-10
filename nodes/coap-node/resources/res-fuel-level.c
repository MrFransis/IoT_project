#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"
#include "./res-fuel-level.h"
#include "../../sensors/utils.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
static int fuel_level_sample;
static int node_id;

EVENT_RESOURCE(res_fuel_level,
        "title =\"Fuel level\";obs",
        res_get_handler,
        NULL,
        NULL,
        NULL,
		res_event_handler);

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char json_response[512];
  int length;

  json_sample(json_response, 512, "fuel_level", fuel_level_sample, "L", node_id);
  printf(json_response);
  length = strlen(json_response);
  memcpy(buffer, json_response, length);

  coap_set_header_content_format(response, APPLICATION_JSON);
  coap_set_payload(response, buffer, length);
}

static void
res_event_handler(void)
{
    // Notify all the observers
    coap_notify_observers(&res_fuel_level);
}

void
res_fuel_level_activate(void)
{
  fuel_level_sample = -1;
  coap_activate_resource(&res_fuel_level, "fuel_level");
}

void
res_fuel_level_update
(int sample, int id)
{
  node_id = id;
  fuel_level_sample = sample;
  res_fuel_level.trigger();
}