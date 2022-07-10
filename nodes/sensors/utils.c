#include "./utils.h"

int
sensor_rand_int(int min, int max)
{
  return ((rand() %(max - min + 1)) + min);
}

void json_sample(char *message_buffer, size_t size, char* topic, int sample, int id)
{
  char unit[4];
  if (strcmp(topic, "temperature") == 0){
    strcpy(unit,"C");
  }else if(strcmp(topic, "fuel_level") == 0){
    strcpy(unit,"L");
  }else{
    strcpy(unit,"W");
  }
  snprintf(message_buffer,
          size,
          "{\"n\":\"%s\",\"v\":\"%d\",\"u\":\"%s\",\"id\":\"%d\"}",
          topic,
          sample,
          unit, 
          id);
}