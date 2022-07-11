package it.unipi.dii.iot.smartgenerator;

import it.unipi.dii.iot.smartgenerator.coap.CoapCollectorServer;
import it.unipi.dii.iot.smartgenerator.mqtt.MqttCollector;
public final class Collector {

    public static void main(String[] args) {
        MqttCollector mqttcollector = new MqttCollector();
        mqttcollector.start();
        
        CoapCollectorServer server = new CoapCollectorServer();
        server.start();
    }
}
