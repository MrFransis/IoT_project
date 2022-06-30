package it.unipi.dii.iot.smartgenerator.coap;

import org.eclipse.californium.core.CoapServer;

public class CoapCollectorServer extends CoapServer {

    public CoapCollectorServer() {
        this.add(new CoapRegistration("coapReg"));
		this.start();
    }
    
}
