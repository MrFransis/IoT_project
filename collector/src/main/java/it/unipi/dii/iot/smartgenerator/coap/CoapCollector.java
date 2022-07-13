package it.unipi.dii.iot.smartgenerator.coap;

import java.util.List;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.CoAP.*;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import com.google.gson.Gson;
import it.unipi.dii.iot.smartgenerator.persistence.MysqlDriver;
import it.unipi.dii.iot.smartgenerator.persistence.MysqlManager;
import it.unipi.dii.iot.smartgenerator.utils.Message;
import it.unipi.dii.iot.smartgenerator.utils.Sensor;

/**
 * CoAP collector.
 * It collects telemetry data saving them in a database,
 * and send allert messages to nodes.
 */
public class CoapCollector {
    CoapClient client;
    CoapClient alertClient;
    MysqlManager mysqlMan;
    CoapObserveRelation relation;
    Boolean temperatureMaxValueExceeded;
    Boolean fuelLevelMinValueExceeded;
    String resource;
    String nodeIp;
    int sensorState;
    String loggingColor;

    public static final int FUEL_LEVEL_THRESHOLD = 251;
    public static final int TEMPERATURE_THRESHOLD = 95;

    public static final int NO_ERROR = 0;
    public static final int FUEL_LEVEL_ERROR = 1;
    public static final int TEMPERATURE_ERROR = 2;

    public static final String[] colors = {"\u001B[91m", "\u001B[92m", "\u001B[93m", "\u001B[94m"}; //red, green, yellow, blue
    public static final String ANSI_RESET = "\u001B[0m";

    public CoapCollector(Sensor s){
        client = new CoapClient(s.getUri());
        nodeIp = s.getNodeIp();
        mysqlMan = new MysqlManager(MysqlDriver.getInstance().openConnection());
        alertClient = new CoapClient("coap://[" + s.getNodeIp() + "]/alert");
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
                loggingColor = colors[msg.getMachineId()%colors.length];
                printToConsole("New measurement from " + nodeIp + " on resource " + resource + ". Value is " + msg.getSample() + msg.getUnit());
                mysqlMan.insertSample(msg);
                
                String topic = msg.getTopic();
                int sample = msg.getSample();
                sensorState = -1;

                switch (topic) {
                    case "fuel_level":
                        if (!fuelLevelMinValueExceeded && sample < FUEL_LEVEL_THRESHOLD) {
                            printToConsole("Fuel level min threshold exceeded!");
                            fuelLevelMinValueExceeded = true;
                            sensorState = FUEL_LEVEL_ERROR;
                        } else if (fuelLevelMinValueExceeded && sample > FUEL_LEVEL_THRESHOLD){
                            printToConsole("Fuel level value has returned to normal");
                            fuelLevelMinValueExceeded = false;
                        }
                        break;
                    case "temperature":
                        if (!temperatureMaxValueExceeded && sample > TEMPERATURE_THRESHOLD) {
                            printToConsole("Temperature max threshold exceeded!");
                            temperatureMaxValueExceeded = true;
                            sensorState = TEMPERATURE_ERROR;
                        } else if (temperatureMaxValueExceeded && sample < TEMPERATURE_THRESHOLD) {
                            printToConsole("Temperature value has returned to normal");
                            temperatureMaxValueExceeded = false;
                            sensorState = NO_ERROR;
                        }
                    default:
                        break;
                }

                if (sensorState != -1) {
                    printToConsole("State has changed, sending POST request to the node with state=" + sensorState + " to " + alertClient.getURI());
                    alertClient.post(new CoapHandler() {

                        @Override
                        public void onLoad(CoapResponse response) {
                            String message = response.getResponseText();
                            ResponseCode code = response.getCode();
                            if (code != ResponseCode.BAD_REQUEST) {
                                int responseSensorState;
                                try {
                                    responseSensorState = Integer.parseInt(message);
                                } catch (NumberFormatException e) {
                                    responseSensorState = -1;
                                }
                                if (responseSensorState != sensorState){
                                    printToConsole("Unable to change status on node with uri " + alertClient.getURI());
                                } else {
                                    printToConsole("Changed status to " + sensorState + " on node with uri " + alertClient.getURI());
                                }
                            } else {
                                printToConsole("400 BAD REQUEST on node with uri " + alertClient.getURI());
                            }
                        }

                        @Override
                        public void onError() {
                            printToConsole("-Failed---");
                        }
                        
                    }, "state=" + sensorState, MediaTypeRegistry.TEXT_PLAIN);
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

    public void printToConsole(String log) {
        System.out.println(loggingColor + "COAP - " + log + ANSI_RESET);
    }
}
