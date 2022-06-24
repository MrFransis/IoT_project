CONTIKI_PROJECT = coap-sensor
all: $(CONTIKI_PROJECT)

CONTIKI=../../../..
SMART_GENERATOR=../..

# Include the CoAP implementation
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap

MODULES_REL += $(SMART_GENERATOR)/nodes/sensors
# Include CoAP resources
MODULES_REL += $(SMART_GENERATOR)/nodes/coap-node/resources

include $(CONTIKI)/Makefile.include
