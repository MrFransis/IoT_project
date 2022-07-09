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
    MysqlManager mysqlMan;
    CoapObserveRelation relation;

    public CoapCollector(Sensor s){
        client = new CoapClient(s.getUri());
        mysqlMan = new MysqlManager(MysqlDriver.getInstance().openConnection());
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
