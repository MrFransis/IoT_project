package it.unipi.dii.iot.smartgenerator.mqtt;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import com.google.gson.Gson;
import com.google.gson.JsonParseException;

import it.unipi.dii.iot.smartgenerator.persistence.MysqlDriver;
import it.unipi.dii.iot.smartgenerator.persistence.MysqlManager;
import it.unipi.dii.iot.smartgenerator.utils.Message;
import it.unipi.dii.iot.smartgenerator.utils.Utils;

public class MqttCollector implements MqttCallback{
    String topic;
    String broker;
    String clientId;
    MqttClient mqttClient;
    MysqlManager mysqlMan;
    List<Integer> tempWarningNodes = new ArrayList<>();
    List<Integer> coolantWarningNodes = new ArrayList<>();
    
    public MqttCollector(){
        Properties configurationParameters = Utils.readConfigurationParameters();
        topic = "#";
        broker = "tcp://"+configurationParameters.getProperty("mqttBrokerIp")+":"+configurationParameters.getProperty("mqttBrokerPort");
        clientId = "JavaCollecotr";
        mysqlMan = new MysqlManager(MysqlDriver.getInstance().openConnection());
    }

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

    public void connectionLost(Throwable cause) {
		// TODO Auto-generated method stub
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
        //System.out.println(String.format("[%s] %s", topic, new String(message.getPayload())));
        try{
            String jsonMessage = new String(message.getPayload());
            Gson gson = new Gson();
            Message msg = gson.fromJson(jsonMessage, Message.class);
            mysqlMan.insertSample(msg); 
            switch(msg.getTopic()) {
                case "temperature":
                    if(msg.getSample() > 160 && !tempWarningNodes.contains((Integer) msg.getMachineId())){
                        publish("temperature", msg.getMachineId());
                        tempWarningNodes.add((Integer) msg.getMachineId());
                    }else if(msg.getSample() < 160 && tempWarningNodes.contains((Integer) msg.getMachineId())){
                        System.out.println("Alert temperature OFFFFF");
                        publish("temperature_off", msg.getMachineId());
                        tempWarningNodes.remove((Integer) msg.getMachineId());
                    }
                    break;
                case "fuel_level":
                    if(msg.getSample() < 25){
                        System.out.println("Alert fuel_level");
                        publish("fuel_level", msg.getMachineId());
                    }
                    break;
                default:
                    break;
              }   
        }catch(Exception e){
            System.out.println(e);
        }
         
	}

	public void deliveryComplete(IMqttDeliveryToken token) {
		// TODO Auto-generated method stub
	}

}
