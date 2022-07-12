#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"
#include "os/dev/leds.h"
#include "../../sensors/utils.h"
#include "../../sensors/energy-generated.h"
#include "../../sensors/fuel-level.h"
#include "../../sensors/temperature.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/*---------------------------------------------------------------------------*/
static uint8_t temperature_state;

#define NO_TEMPERATURE_ERROR    		  0
#define TEMPERATURE_ERROR             1

#define OFF 0
#define ON 1

#define COAP_NO_ERROR                 0
#define COAP_FUEL_LEVEL_ERROR         1
#define COAP_TEMPERATURE_ERROR        2
/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */

RESOURCE(res_alert,
        "title =\"Alert\" POST/PUTstate=<state>;rt=\"Control\"",
        res_get_handler,
        res_post_handler,
        res_post_handler,
        NULL);

static void 
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {

  char res[32] = "";
  snprintf(res, sizeof(res), "%d", temperature_state);
  int length = strlen(res);
  memcpy(buffer, res, length);
  coap_set_header_content_format(response, TEXT_PLAIN);
  coap_set_header_etag(response, (uint8_t *)&length, 1);
  coap_set_payload(response, buffer, length);
}

static void
res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {

  const char *value = NULL;
  int state = -1;
  int success = 1;

  printf("POST request received from the Collector\n");

  if (coap_get_post_variable(request, "state", &value)) {
    state = atoi(value);
    switch (state)
    {
    case COAP_NO_ERROR:
      if (temperature_state == TEMPERATURE_ERROR) {
        process_post(&temperature_sensor_process, TEMPERATURE_EVENT_ALERT, (int*) OFF);
        process_post(&energy_sensor_process, ENERGY_SAMPLE_EVENT, (int*) OFF);
      }
      leds_off(LEDS_RED);
      leds_on(LEDS_BLUE);
      printf("Temperature value has returned to normal. Changing led color to blue\n");
      temperature_state = NO_TEMPERATURE_ERROR;
      break;
    case COAP_TEMPERATURE_ERROR:
      process_post(&temperature_sensor_process, TEMPERATURE_EVENT_ALERT, (int*) ON);
      process_post(&energy_sensor_process, ENERGY_SAMPLE_EVENT, (int*) ON);
      leds_off(LEDS_BLUE);
      leds_on(LEDS_RED);
      printf("Temperature max threshold exceeded! Changing led color to red\n");
      temperature_state = TEMPERATURE_ERROR;
      break;
    case COAP_FUEL_LEVEL_ERROR:
      leds_on(LEDS_LED2);
      printf("Fuel level min threshold exceeded! Activating secondary led\n");
      break;
    default:
      success = 0;
      break;
    }
  } else {
    success = 0;
  }

  if(!success) {
    coap_set_status_code(response, BAD_REQUEST_4_00);
    printf("400 BAD REQUEST\n");
  } else {
    char res[32] = "";
    snprintf(res, sizeof(res), "%d", state);
    int length = strlen(res);
    memcpy(buffer, res, length);
    printf("Post request processed successfully\n");
    coap_set_header_content_format(response, TEXT_PLAIN);
    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, buffer, length);
  }
}


void
res_alert_activate(void)
{
  temperature_state = NO_TEMPERATURE_ERROR;
  coap_activate_resource(&res_alert, "alert");
}