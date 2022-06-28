package it.unipi.dii.iot.smartgenerator;

import java.util.Calendar;

import it.unipi.dii.iot.smartgenerator.coap.CoapCollector;
import it.unipi.dii.iot.smartgenerator.coap.CoapCollectorServer;
import it.unipi.dii.iot.smartgenerator.mqtt.MqttCollector;
import it.unipi.dii.iot.smartgenerator.persistence.*;
import org.apache.commons.cli.*;
import org.eclipse.paho.client.mqttv3.MqttException;
public final class Collector {

    private static void runCoapServer() {
		new Thread() {
			public void run() {
				CoapCollectorServer server = new CoapCollectorServer();
			}
		}.start();
	}
    public static void main(String[] args) {
        MqttCollector mqttcollector = new MqttCollector();
        mqttcollector.start();
        
        //CoapCollector coapcollector = new CoapCollector();
        //coapcollector.startObserving();
        runCoapServer();
        
        while(true){

        }
    }
}
