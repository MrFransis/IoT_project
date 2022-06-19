package it.unipi.dii.iot.smartgenerator.coap;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.CoAP.*;
import org.eclipse.californium.core.coap.Request;

public class CoapCollector {
    CoapClient client;

    public CoapCollector(){
        client = new CoapClient("coap://[" + "]/");
    }

    public void onError() {
        System.err.println("-Failed---");
    }

    public void startObserving(){
        CoapObserveRelation relation = client.observe(new CoapHandler() {
            public void onLoad(CoapResponse response) {
                String content = response.getResponseText();
                System.out.println(content);
            }

            public void onError() {
                System.err.println("-Failed---");
            }
        });
    }

    //public cancelObserving(){
    //    relation.proactiveCancel();
    //}
}
