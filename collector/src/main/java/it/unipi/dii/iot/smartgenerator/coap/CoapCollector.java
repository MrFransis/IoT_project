package it.unipi.dii.iot.smartgenerator.coap;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.CoAP.*;
import org.eclipse.californium.core.coap.Request;

import com.google.gson.Gson;
import com.google.gson.JsonParseException;

import it.unipi.dii.iot.smartgenerator.persistence.MysqlDriver;
import it.unipi.dii.iot.smartgenerator.persistence.MysqlManager;
import it.unipi.dii.iot.smartgenerator.utils.Message;
import it.unipi.dii.iot.smartgenerator.utils.Sensor;
import it.unipi.dii.iot.smartgenerator.utils.Utils;

public class CoapCollector {
    CoapClient client;
    CoapClient alertClient;
    MysqlManager mysqlMan;
    CoapObserveRelation relation;
    boolean sensorMaxValueExceeded;
    String resource;

    public static final int COOLANT_LEVEL_THRESHOLD = 30;
    public static final int COOLANT_TEMPERATURE_THRESHOLD = 70;
    public static final int FUEL_LEVEL_THRESHOLD = 800;
    public static final int TEMPERATURE_THRESHOLD = 150;

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
                Gson gson = new Gson();
                Message msg = gson.fromJson(jsonMessage, Message.class);
                mysqlMan.insertSample(msg);
                if (resource == "coolant_temperature") {
                    // do something
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
