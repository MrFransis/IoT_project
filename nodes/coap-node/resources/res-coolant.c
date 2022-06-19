#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);
static void buildJsonResponse(char* json_response,int sample,int unit, int machineid);

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
EVENT_RESOURCE(res_obs,
         "title=\"Hello world\";rt=\"Text\"",
         res_get_handler,
         NULL,
         NULL,
         NULL, 
		 res_event_handler);

char json_response [200];
int length;

static void
res_event_handler(void)
{
    // Notify all the observers
    coap_notify_observers(&res_obs);
}

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  buildJsonResponse(json_response,10,1,111);
  length=strlen(json_response);
  memcpy(buffer, json_response, length);

  coap_set_header_content_format(response, TEXT_PLAIN);
  coap_set_payload(response, buffer, length);
}

static void buildJsonResponse(char* json_response,int sample,int unit, int machineid){
	//{sample,unit,machineid} 
	sprintf(json_response,"{\"sample\":\"%d\",\"unit\":\"%d\",\"machineid\":\"%d\"}",sample,unit,machineid);
	
}