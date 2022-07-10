package it.unipi.dii.iot.smartgenerator.mqtt;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
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
            mqttClient.subscribe(topic);    
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
        System.out.println(String.format("[%s] %s", topic, new String(message.getPayload())));
        System.out.println("1");
        String jsonMessage = new String(message.getPayload());
        System.out.println("2");
        Gson gson = new Gson();
        System.out.println("3");
        Message msg = gson.fromJson(jsonMessage, Message.class);
        System.out.println("4");
        mysqlMan.insertSample(msg);
        System.out.println("sono qui");
        switch(msg.getTopic()) {
            case "temperature":
                if(msg.getSample() > 160 && !tempWarningNodes.contains((Integer) msg.getMachineId())){
                    publish("temperature", msg.getMachineId());
                    tempWarningNodes.add((Integer) msg.getMachineId());
                }else if(msg.getSample() < 160 && tempWarningNodes.contains((Integer) msg.getMachineId())){
                    publish("temperature_off", msg.getMachineId());
                    tempWarningNodes.remove((Integer) msg.getMachineId());
                }
                break;
            case "fuel_level":
                if(msg.getSample() < 25){
                    publish("fuel_level", msg.getMachineId());
                }
                break;
            case "coolant_temperature":
                if(msg.getSample() > 70 && !coolantWarningNodes.contains((Integer) msg.getMachineId())){
                    publish("coolant_temperature", msg.getMachineId());
                    coolantWarningNodes.add((Integer) msg.getMachineId());
                }else if(msg.getSample() < 70 && coolantWarningNodes.contains((Integer) msg.getMachineId())){
                    publish("coolant_temperature_off", msg.getMachineId());
                    coolantWarningNodes.remove((Integer) msg.getMachineId());
                }
                break;
            case "coolant":
                if(msg.getSample() < 25){
                    publish("coolant", msg.getMachineId());
                }
                break;
            default:
              // code block
          }
	}

	public void deliveryComplete(IMqttDeliveryToken token) {
		// TODO Auto-generated method stub
	}

}
