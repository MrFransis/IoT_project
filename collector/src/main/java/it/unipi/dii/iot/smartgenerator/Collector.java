package it.unipi.dii.iot.smartgenerator;

import java.util.Calendar;

import it.unipi.dii.iot.smartgenerator.mqtt.MqttCollector;
import it.unipi.dii.iot.smartgenerator.persistence.*;
import org.apache.commons.cli.*;
import org.eclipse.paho.client.mqttv3.MqttException;
public final class Collector {

    public static void main(String[] args) {
        MqttCollector mqttcollector = new MqttCollector();
        mqttcollector.start();
        
        while(true){

        }
    }
}
