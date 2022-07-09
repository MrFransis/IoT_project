package it.unipi.dii.iot.smartgenerator;

import java.util.Calendar;

import it.unipi.dii.iot.smartgenerator.coap.CoapCollector;
import it.unipi.dii.iot.smartgenerator.coap.CoapCollectorServer;
import it.unipi.dii.iot.smartgenerator.mqtt.MqttCollector;
import org.apache.commons.cli.*;
import org.eclipse.paho.client.mqttv3.MqttException;
public final class Collector {

    public static void main(String[] args) {
        MqttCollector mqttcollector = new MqttCollector();
        mqttcollector.start();
        
        CoapCollectorServer server = new CoapCollectorServer();
        server.start();
    }
}
