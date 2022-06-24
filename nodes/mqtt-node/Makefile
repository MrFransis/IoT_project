CONTIKI_PROJECT = LAB-mqtt
all: $(CONTIKI_PROJECT)

CONTIKI=../../../..
SMART_GENERATOR=../..

include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/mqtt

-include $(CONTIKI)/Makefile.identify-target

MODULES_REL += arch/platform/$(TARGET)
MODULES_REL += $(SMART_GENERATOR)/nodes/sensors


include $(CONTIKI)/Makefile.include
