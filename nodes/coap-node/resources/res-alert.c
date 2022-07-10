#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"
#include "dev/etc/rgb-led/rgb-led.h"
#include "../../sensors/utils.h"
#include "../../sensors/energy-generated.h"
#include "../../sensors/fuel-level.h"
#include "../../sensors/temperature.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/*---------------------------------------------------------------------------*/
static uint8_t state;

#define NO_ERROR    		              0
#define FUEL_LEVEL_ERROR              1
#define TEMPERATURE_ERROR             2

#define OFF 0
#define ON 1
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
  snprintf(res, sizeof(res), "%d", state);
  int length = strlen(res);
  memcpy(buffer, res, length);
  coap_set_header_content_format(response, TEXT_PLAIN);
  coap_set_header_etag(response, (uint8_t *)&length, 1);
  coap_set_payload(response, buffer, length);
}

static void
res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {

  const char *value = NULL;
  int postState;
  int success = 1;

  if (coap_get_post_variable(request, "state", &value)) {
    postState = atoi(value);
    if (postState >= 0 && postState < 5) {
      if (postState != state) {
        if (postState == NO_ERROR) {
          printf("Riattivo sensori...\n");
          int data = OFF;
          if (state == TEMPERATURE_ERROR) {
            process_post(&temperature_sensor_process, TEMPERATURE_EVENT_ALERT, &data);
          }
          //rimetto led verde
          //rgb_led_set(RGB_LED_GREEN);
          printf("Led verde\n");
        } else {
          printf("Disattivo sensori e cambio colore ai led...\n");
          int data = ON;
          switch (postState)
          {
          case TEMPERATURE_ERROR:
            process_post(&temperature_sensor_process, TEMPERATURE_EVENT_ALERT, &data);
            //rgb_led_set(RGB_LED_RED);
            printf("Led rosso\n");
            break;
          case FUEL_LEVEL_ERROR:
            //rgb_led_set(RGB_LED_YELLOW);
            printf("Led giallo\n");
            break;
          default:
            break;
          }
        }
        state = postState;
      }
    } else {
      success = 0;
    }
  }

  if(!success) {
    coap_set_status_code(response, BAD_REQUEST_4_00);
  } else {
    char res[32] = "";
    snprintf(res, sizeof(res), "%d", state);
    int length = strlen(res);
    memcpy(buffer, res, length);
    coap_set_header_content_format(response, TEXT_PLAIN);
    coap_set_header_etag(response, (uint8_t *)&length, 1);
    coap_set_payload(response, buffer, length);
  }
}


void
res_battery_voltage_activate(void)
{
  state = ON;
  coap_activate_resource(&res_alert, "alert");
}