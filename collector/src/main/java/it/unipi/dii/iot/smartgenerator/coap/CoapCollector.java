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
    Boolean sensorMaxValueExceeded;
    String resource;

    public static final int COOLANT_LEVEL_THRESHOLD = 30;
    public static final int COOLANT_TEMPERATURE_THRESHOLD = 70;
    public static final int FUEL_LEVEL_THRESHOLD = 800;
    public static final int TEMPERATURE_THRESHOLD = 150;

    public static final int NO_ERROR = 0;
    public static final int COOLANT_TEMPERATURE_ERROR = 1;
    public static final int COOLANT_LEVEL_ERROR = 2;
    public static final int FUEL_LEVEL_ERROR = 3;
    public static final int TEMPERATURE_ERROR = 4;

    public CoapCollector(Sensor s){
        client = new CoapClient(s.getUri());
        mysqlMan = new MysqlManager(MysqlDriver.getInstance().openConnection());
        alertClient = new CoapClient("coap://[" + s.getNodeIp() + "]/alert");
        sensorMaxValueExceeded = false;
        resource = s.getResourcePath();
    }

    public void onError() {
        System.err.println("-Failed---");
    }

    public void startObserving(){
            relation = client.observe(new CoapHandler() {
            public void onLoad(CoapResponse response) {
                String jsonMessage = new String(response.getResponseText());
                System.out.println("Messaggio " + jsonMessage);
                Gson gson = new Gson();
                Message msg = gson.fromJson(jsonMessage, Message.class);
                mysqlMan.insertSample(msg);
                
                String topic = msg.getTopic();
                int sample = msg.getSample();
                Boolean currentMaxValueExceeded = false;
                int sensorState = -1;

                switch (topic) {
                    case "coolant_temperature":
                        if (sample > COOLANT_TEMPERATURE_THRESHOLD) {
                            currentMaxValueExceeded = true;
                            sensorMaxValueExceeded = true;
                            sensorState = COOLANT_TEMPERATURE_ERROR;
                        }
                        break;
                    case "coolant":
                        if (sample > COOLANT_LEVEL_THRESHOLD) {
                            currentMaxValueExceeded = true;
                            sensorMaxValueExceeded = true;
                            sensorState = COOLANT_LEVEL_ERROR;
                        }
                        break;
                    case "fuel_level":
                        if (sample > FUEL_LEVEL_THRESHOLD) {
                            currentMaxValueExceeded = true;
                            sensorMaxValueExceeded = true;
                            sensorState = FUEL_LEVEL_ERROR;
                        }
                        break;
                    case "temperature":
                        if (sample > TEMPERATURE_THRESHOLD) {
                            currentMaxValueExceeded = true;
                            sensorMaxValueExceeded = true;
                            sensorState = TEMPERATURE_ERROR;
                        }
                    default:
                        break;
                }

                // sensor value has returned to normal
                if (!currentMaxValueExceeded && sensorMaxValueExceeded) {
                    sensorMaxValueExceeded = false;
                    sensorState = NO_ERROR;
                }

                if (sensorState != -1) {
                    CoapResponse postResponse = alertClient.post("state=" + sensorState, MediaTypeRegistry.TEXT_PLAIN);
                    if (postResponse.getCode() != ResponseCode.BAD_REQUEST) {
                        if (Integer.parseInt(postResponse.getResponseText()) != sensorState) {
                            System.err.println("Unable to change status on node with uri " + client.getURI());
                        }
                    } else {
                        System.err.println("400 BAD REQUEST on node with uri " + client.getURI());
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
