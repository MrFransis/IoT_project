#include "contiki.h"
#include "net/routing/routing.h"
#include "mqtt.h"
#include "net/ipv6/uip.h"
#include "net/ipv6/uip-icmp6.h"
#include "net/ipv6/sicslowpan.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "lib/sensors.h"
#include "dev/button-hal.h"
#include "dev/etc/rgb-led/rgb-led.h"
#include "os/sys/log.h"
#include "mqtt-sensor.h"
#include "../sensors/coolant-level.h"
#include "../sensors/coolant-temperature.h"
#include "../sensors/energy-generated.h"
#include "../sensors/fuel-level.h"
#include "../sensors/motor-rpm.h"
#include "../sensors/temperature.h"
#include "../sensors/utils.h"
//#include "ieee-addr.h"

#include <string.h>
#include <strings.h>
/*---------------------------------------------------------------------------*/
#define LOG_MODULE "mqtt-sensor"
#ifdef MQTT_CLIENT_CONF_LOG_LEVEL
#define LOG_LEVEL MQTT_CLIENT_CONF_LOG_LEVEL
#else
#define LOG_LEVEL LOG_LEVEL_DBG
#endif

/*---------------------------------------------------------------------------*/
/* MQTT broker address. */
#define MQTT_CLIENT_BROKER_IP_ADDR "fd00::1"

static const char *broker_ip = MQTT_CLIENT_BROKER_IP_ADDR;

// Defaukt config values
#define DEFAULT_BROKER_PORT         1883
#define DEFAULT_PUBLISH_INTERVAL    (30 * CLOCK_SECOND)

// We assume that the broker does not require authentication

/*---------------------------------------------------------------------------*/
/* Various states */
static uint8_t state;

#define STATE_INIT    		  0
#define STATE_NET_OK    	  1
#define STATE_CONNECTING      2
#define STATE_CONNECTED       3
#define STATE_SUBSCRIBED      4
#define STATE_DISCONNECTED    5

/*---------------------------------------------------------------------------*/
PROCESS_NAME(mqtt_client_process);
AUTOSTART_PROCESSES(&mqtt_client_process);
/*---------------------------------------------------------------------------*/

/* Maximum TCP segment size for outgoing segments of our socket */
#define MAX_TCP_SEGMENT_SIZE    32
#define CONFIG_IP_ADDR_STR_LEN   64

static mqtt_status_t status;
static char broker_address[CONFIG_IP_ADDR_STR_LEN];
/*---------------------------------------------------------------------------*/
/*
 * Buffers for Client ID and Topics.
 * Make sure they are large enough to hold the entire respective string
 */
#define BUFFER_SIZE 64

static char client_id[BUFFER_SIZE];
static char sub_topic[BUFFER_SIZE];

static int node_id;

// Periodic timer to check the state of the MQTT client
#define STATE_MACHINE_PERIODIC     (CLOCK_SECOND >> 1)
static struct etimer periodic_timer;

/*---------------------------------------------------------------------------*/
/*
 * The main MQTT buffers.
 * We will need to increase if we start publishing more data.
 */
#define APP_BUFFER_SIZE 512
static char app_buffer[APP_BUFFER_SIZE];
/*---------------------------------------------------------------------------*/
static struct mqtt_message *msg_ptr = 0;

static struct mqtt_connection conn;

/*---------------------------------------------------------------------------*/
PROCESS(mqtt_client_process, "MQTT Client");

/*---------------------------------------------------------------------------*/
static void
pub_handler(const char *topic, uint16_t topic_len, const uint8_t *chunk,
            uint16_t chunk_len)
{
  printf("Pub Handler: topic='%s' (len=%u), chunk_len=%u\n", topic,
          topic_len, chunk_len);
  if(strcmp(topic, sub_topic) == 0) {
    printf("Received Actuator command\n");
	  printf("%s\n", chunk);
    if(strcmp((const char *)chunk, "coolant_temp")==0){
      //rgb_led_set(RGB_LED_RED);
      printf("1\n");
    }else if (strcmp((const char *)chunk, "temperature")==0){
      //rgb_led_set(RGB_LED_RED);
      printf("2\n");
    }else if (strcmp((const char *)chunk, "fuel_lvl")==0){
      //rgb_led_set(RGB_LED_BLUE);
      printf("3\n");
    }else if (strcmp((const char *)chunk, "coolant_lvl")==0){
      //rgb_led_set(RGB_LED_RED);
      printf("4\n");
    }else{
        printf("UNKNOWN COMMAND\n");
    }
    return;
  }
}
/*---------------------------------------------------------------------------*/
static void 
publish(char* pub_topic, int sample)
{
  json_sample(app_buffer, APP_BUFFER_SIZE, "temperature", sample, "C", node_id);
  int status = mqtt_publish(&conn, NULL, pub_topic, (uint8_t *)app_buffer, strlen(app_buffer), MQTT_QOS_LEVEL_0, MQTT_RETAIN_OFF);
  
  switch(status) {
  case MQTT_STATUS_OK:
    return;
  case MQTT_STATUS_NOT_CONNECTED_ERROR: {
    printf("Publishing failed. Error: MQTT_STATUS_NOT_CONNECTED_ERROR.\n");
    return;
  }
  case MQTT_STATUS_OUT_QUEUE_FULL: {
    printf("Publishing failed. Error: MQTT_STATUS_OUT_QUEUE_FULL.\n");
    break;
  }
  default:
    printf("Publishing failed. Error: unknown.\n");
    return;
  }
}

static void 
sensors_emulation(process_event_t event, int sample)
{
  printf("%d /n", sample);
  //Engine temperature 
  if(event == TEMPERATURE_SAMPLE_EVENT){
    publish("temperature", sample);
  }else if(event == MOTOR_RPM_SAMPLE_EVENT){
    publish("rpm", sample);
  }else if(event == FUEL_LEVEL_SAMPLE_EVENT){
    publish("fuel_level", sample);
  }else if(event == ENERGY_SAMPLE_EVENT){
    publish("energy_generated", sample);
  }else if(event == COOLANT_TEMPERATURE_SAMPLE_EVENT){
    publish("coolant_temperature", sample);
  }else if(event == COOLANT_SAMPLE_EVENT){
    publish("coolant", sample);
  }
}

static void
load_sensors_processes()
{
  process_start(&temperature_sensor_process, NULL);
  process_post(&temperature_sensor_process, TEMPERATURE_EVENT_SUB, &mqtt_client_process);

  process_start(&motor_rpm_sensor_process, NULL);
  process_post(&motor_rpm_sensor_process, MOTOR_RPM_EVENT_SUB, &mqtt_client_process);

  process_start(&fuel_level_sensor_process, NULL);
  process_post(&fuel_level_sensor_process, FUEL_LEVEL_EVENT_SUB, &mqtt_client_process);

  process_start(&energy_sensor_process, NULL);
  process_post(&energy_sensor_process, ENERGY_EVENT_SUB, &mqtt_client_process);

  process_start(&coolant_temperature_sensor_process, NULL);
  process_post(&coolant_temperature_sensor_process, COOLANT_TEMPERATURE_EVENT_SUB, &mqtt_client_process);

  process_start(&coolant_sensor_process, NULL);
  process_post(&coolant_sensor_process, COOLANT_EVENT_SUB, &mqtt_client_process);
}

static bool
sensor_event(process_event_t event)
{
    if(event == TEMPERATURE_SAMPLE_EVENT
       || event == MOTOR_RPM_SAMPLE_EVENT
       || event == FUEL_LEVEL_SAMPLE_EVENT
       || event == ENERGY_SAMPLE_EVENT
       || event == COOLANT_TEMPERATURE_SAMPLE_EVENT
       || event == COOLANT_SAMPLE_EVENT) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
static void
mqtt_event(struct mqtt_connection *m, mqtt_event_t event, void *data)
{
  switch(event) {
  case MQTT_EVENT_CONNECTED: {
    printf("Application has a MQTT connection\n");

    state = STATE_CONNECTED;
    break;
  }
  case MQTT_EVENT_DISCONNECTED: {
    printf("MQTT Disconnect. Reason %u\n", *((mqtt_event_t *)data));

    state = STATE_DISCONNECTED;
    process_poll(&mqtt_client_process);
    break;
  }
  case MQTT_EVENT_PUBLISH: {
    msg_ptr = data;

    pub_handler(msg_ptr->topic, strlen(msg_ptr->topic),
                msg_ptr->payload_chunk, msg_ptr->payload_length);
    break;
  }
  case MQTT_EVENT_SUBACK: {
#if MQTT_311
    mqtt_suback_event_t *suback_event = (mqtt_suback_event_t *)data;

    if(suback_event->success) {
      printf("Application is subscribed to topic successfully\n");
    } else {
      printf("Application failed to subscribe to topic (ret code %x)\n", suback_event->return_code);
    }
#else
    printf("Application is subscribed to topic successfully\n");
#endif
    break;
  }
  case MQTT_EVENT_UNSUBACK: {
    printf("Application is unsubscribed to topic successfully\n");
    break;
  }
  case MQTT_EVENT_PUBACK: {
    printf("Publishing complete.\n");
    break;
  }
  default:
    printf("Application got a unhandled MQTT event: %i\n", event);
    break;
  }
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

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mqtt_client_process, ev, data)
{

  PROCESS_BEGIN();
  printf("MQTT Client Process\n");

  // Initialize the ClientID as MAC address
  snprintf(client_id, BUFFER_SIZE, "%02x%02x%02x%02x%02x%02x",
                     linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
                     linkaddr_node_addr.u8[2], linkaddr_node_addr.u8[5],
                     linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);

  // Broker registration					 
  mqtt_register(&conn, &mqtt_client_process, client_id, mqtt_event,
                  MAX_TCP_SEGMENT_SIZE);

	  
  node_id = linkaddr_node_addr.u8[7];
  //node_id = IEEE_ADDR_NODE_ID

  state=STATE_INIT;
				    
  // Initialize periodic timer to check the status 
  etimer_set(&periodic_timer, STATE_MACHINE_PERIODIC);

  /* Main loop */
  while(1) {

    PROCESS_YIELD();
    if((ev == PROCESS_EVENT_TIMER && data == &periodic_timer) || 
	      ev == PROCESS_EVENT_POLL){
			  			  
		  if(state==STATE_INIT){
			 if(have_connectivity()==true)  
				 state = STATE_NET_OK;
		  } 
		  
		  if(state == STATE_NET_OK){
			  // Connect to MQTT server
			  printf("Connecting!\n");
			  
			  memcpy(broker_address, broker_ip, strlen(broker_ip));
			  
			  mqtt_connect(&conn, broker_address, DEFAULT_BROKER_PORT,
						   (DEFAULT_PUBLISH_INTERVAL * 3) / CLOCK_SECOND,
						   MQTT_CLEAN_SESSION_ON);
			  state = STATE_CONNECTING;
		  }
		  
		  if(state==STATE_CONNECTED){
        //start sensors
        load_sensors_processes();
        printf("ok\n");
        //led color to operational
        //rgb_led_set(RGB_LED_GREEN);

			  // Subscribe to a topic
        char topic[64];
        sprintf(topic, "alarm_%d", node_id);
        strcpy(sub_topic,topic);

			  status = mqtt_subscribe(&conn, NULL, sub_topic, MQTT_QOS_LEVEL_0);

			  printf("Subscribing!\n");
			  if(status == MQTT_STATUS_OUT_QUEUE_FULL) {
				LOG_ERR("Tried to subscribe but command queue was full!\n");
				PROCESS_EXIT();
			  }
			  
			  state = STATE_SUBSCRIBED;
		  }else if ( state == STATE_DISCONNECTED ){
        LOG_ERR("Disconnected form MQTT broker\n");	
        state = STATE_INIT;
        // Recover from error
      }
      
      etimer_set(&periodic_timer, STATE_MACHINE_PERIODIC);
      
    }
    if(sensor_event(ev) && state == STATE_SUBSCRIBED){
        // Publish something
        sensors_emulation(ev, *((int *)data));
    } 

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
