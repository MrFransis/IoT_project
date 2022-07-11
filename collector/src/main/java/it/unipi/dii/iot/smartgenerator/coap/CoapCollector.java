package it.unipi.dii.iot.smartgenerator.coap;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.CoAP.*;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.core.coap.Request;

import com.google.gson.Gson;
import com.google.gson.JsonParseException;

import it.unipi.dii.iot.smartgenerator.persistence.MysqlDriver;
import it.unipi.dii.iot.smartgenerator.persistence.MysqlManager;
import it.unipi.dii.iot.smartgenerator.utils.Message;
import it.unipi.dii.iot.smartgenerator.utils.NodeState;
import it.unipi.dii.iot.smartgenerator.utils.Sensor;
import it.unipi.dii.iot.smartgenerator.utils.Utils;

public class CoapCollector {
    CoapClient client;
    CoapClient alertClient;
    MysqlManager mysqlMan;
    CoapObserveRelation relation;
    Boolean temperatureMaxValueExceeded;
    Boolean fuelLevelMinValueExceeded;
    String resource;

    public static final int FUEL_LEVEL_THRESHOLD = 25;
    public static final int TEMPERATURE_THRESHOLD = 160;

    public static final int NO_ERROR = 0;
    public static final int FUEL_LEVEL_ERROR = 1;
    public static final int TEMPERATURE_ERROR = 2;

    public CoapCollector(Sensor s){
        client = new CoapClient(s.getUri());
        mysqlMan = new MysqlManager(MysqlDriver.getInstance().openConnection());
        alertClient = new CoapClient("coap://[" + s.getNodeIp() + "]/alert");
        System.out.println(alertClient);
        temperatureMaxValueExceeded = false;
        fuelLevelMinValueExceeded = false;
        resource = s.getResourcePath();
    }

    public void onError() {
        System.err.println("-Failed---");
    }

    public void startObserving(){
            relation = client.observe(new CoapHandler() {
            public void onLoad(CoapResponse response) {
                String jsonMessage = new String(response.getResponseText());
                Gson gson = new Gson();
                Message msg = gson.fromJson(jsonMessage, Message.class);
                if (msg.getSample() == -1) {
                    return;
                }
                System.out.println("COAP SERVER - New measurement from " + client.getURI() + " on resource " + resource + ". Value is " + msg.getSample() + msg.getUnit());

                mysqlMan.insertSample(msg);
                
                String topic = msg.getTopic();
                int sample = msg.getSample();
                int sensorState = -1;

                switch (topic) {
                    case "fuel_level":
                        if (sample < FUEL_LEVEL_THRESHOLD) {
                            System.out.println("Fuel level min threshold exceeded!");
                            fuelLevelMinValueExceeded = true;
                            sensorState = FUEL_LEVEL_ERROR;
                        } else if (fuelLevelMinValueExceeded){
                            System.out.println("Fuel level value has returned to normal");
                            fuelLevelMinValueExceeded = false;
                        }
                        break;
                    case "temperature":
                        if (sample > 150) {
                            System.out.println("Temperature max threshold exceeded!");
                            temperatureMaxValueExceeded = true;
                            sensorState = TEMPERATURE_ERROR;
                        } else if (temperatureMaxValueExceeded) {
                            System.out.println("Temperature value has returned to normal");
                            temperatureMaxValueExceeded = false;
                            sensorState = NO_ERROR;
                        }
                    default:
                        break;
                }

                if (sensorState != -1) {
                    System.out.println("State has changed, sending POST request to the node with state=" + sensorState + " to " + alertClient.getURI());
                    Request postRequest = new Request(Code.POST);
                    postRequest.setPayload("state=" + sensorState);
                    CoapResponse postResponse = alertClient.advanced(postRequest);
                    //CoapResponse postResponse = alertClient.post("state=" + sensorState, MediaTypeRegistry.TEXT_PLAIN);
                    if (postResponse.getCode() != ResponseCode.BAD_REQUEST) {
                        if (Integer.parseInt(postResponse.getResponseText()) != sensorState) {
                            System.err.println("Unable to change status on node with uri " + alertClient.getURI());
                        }
                    } else {
                        System.err.println("400 BAD REQUEST on node with uri " + alertClient.getURI());
                    }
                }
            }

            public void onError() {
                System.err.println("-Failed---");
            }
        });
    }

    public void cancelObserving(){
        relation.proactiveCancel();
    }
}
