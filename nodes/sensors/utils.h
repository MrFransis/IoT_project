#ifndef UTILS_H_
#define UTILS_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEMPERATURE_UNIT                  "C"
#define COOLANT_UNIT                      "L"

int sensor_rand_int(int min, int max);

void json_sample(char *message_buffer, size_t size, char* topic, int sample, int id);

#endif