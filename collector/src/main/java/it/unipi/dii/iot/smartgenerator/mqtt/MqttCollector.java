package it.unipi.dii.iot.smartgenerator.mqtt;

import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import com.google.gson.Gson;
import com.google.gson.JsonParseException;
import it.unipi.dii.iot.smartgenerator.persistence.MysqlDriver;
import it.unipi.dii.iot.smartgenerator.persistence.MysqlManager;
import it.unipi.dii.iot.smartgenerator.utils.Message;
import it.unipi.dii.iot.smartgenerator.utils.Utils;

/**
 * MQTT collector.
 * It collects telemetry data saving them in a database,
 * and send allert messages to nodes.
 */
public class MqttCollector implements MqttCallback{
    String topic;
    String broker;
    String clientId;
    MqttClient mqttClient;
    MysqlManager mysqlMan;
    String loggingColor;
    List<Integer> tempWarningNodes = new ArrayList<>();
    List<Integer> fuelWarningNodes = new ArrayList<>();
   
    public static final int FUEL_LEVEL_THRESHOLD = 251;
    public static final int TEMPERATURE_THRESHOLD = 95;
    public static final String NO_ERROR = "0";
    public static final String FUEL_LEVEL_ERROR = "1";
    public static final String TEMPERATURE_ERROR = "2";

    public static final String[] colors = {"\u001B[95m", "\u001B[96m"}; //purple, cyan
    public static final String ANSI_RESET = "\u001B[0m";
    /**
     * Creates a new MQTT collector.
     */
    public MqttCollector(){
        Properties configurationParameters = Utils.readConfigurationParameters();
        topic = "#";
        broker = "tcp://"+configurationParameters.getProperty("mqttBrokerIp")+":"+configurationParameters.getProperty("mqttBrokerPort");
        clientId = "JavaCollector";
        mysqlMan = new MysqlManager(MysqlDriver.getInstance().openConnection());
    }

    /**
     * Starts the MQTT collector and subscribes to the topics of interest.
     */
    public void start(){
        try {
            mqttClient = new MqttClient(broker, clientId);
            System.out.println("Connecting to broker: "+ broker);
            mqttClient.setCallback(this);
            mqttClient.connect();
            mqttClient.subscribe("fuel_level");
            mqttClient.subscribe("energy_generated");
            mqttClient.subscribe("temperature");
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void stop(){
        try {
            mqttClient.close(true);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void publish(String content, int node){
        try{
            MqttMessage message = new MqttMessage(content.getBytes());
            this.mqttClient.publish("alarm_" + node, message);
        }catch(MqttException e){
            e.printStackTrace();
        }
    }

    public void messageArrived(String topic, MqttMessage message) throws Exception {
        try{
            String jsonMessage = new String(message.getPayload());
            Gson gson = new Gson();
            Message msg = gson.fromJson(jsonMessage, Message.class);

            loggingColor = colors[msg.getMachineId()%colors.length];
            printToConsole("New measurement from machine " + msg.getMachineId() + " on topic " + topic + ". Value is " + msg.getSample() + msg.getUnit());

            mysqlMan.insertSample(msg);

            switch(msg.getTopic()) {
                case "temperature":
                    if(msg.getSample() > TEMPERATURE_THRESHOLD && !tempWarningNodes.contains((Integer) msg.getMachineId())){
                        printToConsole("Temperature max threshold exceeded! Sending alarm msg to node: " + msg.getMachineId());
                        publish(TEMPERATURE_ERROR, msg.getMachineId());
                        tempWarningNodes.add((Integer) msg.getMachineId());
                    }else if(msg.getSample() < TEMPERATURE_THRESHOLD && tempWarningNodes.contains((Integer) msg.getMachineId())){
                        printToConsole("Temperature value has returned to normal");
                        publish(NO_ERROR, msg.getMachineId());
                        tempWarningNodes.remove((Integer) msg.getMachineId());
                    }
                    break;
                case "fuel_level":
                    if(msg.getSample() < FUEL_LEVEL_THRESHOLD && !fuelWarningNodes.contains((Integer) msg.getMachineId())){
                        printToConsole("Fuel level min threshold exceeded! Sending alarm msg to node: " + msg.getMachineId());
                        publish(FUEL_LEVEL_ERROR, msg.getMachineId());
                        fuelWarningNodes.add((Integer) msg.getMachineId());
                    }else if(msg.getSample() > FUEL_LEVEL_THRESHOLD && fuelWarningNodes.contains((Integer) msg.getMachineId())){
                        printToConsole("Fuel level value has returned to normal");
                        fuelWarningNodes.remove((Integer) msg.getMachineId());
                    }
                    break;
                default:
                    break;
              }   
        }catch(JsonParseException e){
            System.out.println(e);
        }
         
	}

    @Override
    public void connectionLost(Throwable cause) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        // TODO Auto-generated method stub
        
    }

    public void printToConsole(String log) {
        System.out.println(loggingColor + "MQTT - " + log + ANSI_RESET);
    }
}
