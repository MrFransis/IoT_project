CONTIKI_PROJECT = mqtt-sensor
all: $(CONTIKI_PROJECT)

CONTIKI=../../../..
SMART_GENERATOR=../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/mqtt

MODULES_REL += arch/platform/$(TARGET)
MODULES_REL += $(SMART_GENERATOR)/nodes/sensors


include $(CONTIKI)/Makefile.include
