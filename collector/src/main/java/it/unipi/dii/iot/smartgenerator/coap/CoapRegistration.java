package it.unipi.dii.iot.smartgenerator.coap;

import java.net.InetAddress;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.server.resources.CoapExchange;

import it.unipi.dii.iot.smartgenerator.utils.Sensor;

public class CoapRegistration extends CoapResource{

    public CoapRegistration(String name) {
        super(name);
    }
    
    public void handleGet(CoapExchange exchange){
        exchange.accept();

        InetAddress sensorIp = exchange.getSourceAddress();
        CoapClient client = new CoapClient("coap://[" + sensorIp.getHostAddress() + "]:5683/.well-known/core");
        CoapResponse response = client.get();
    
        String code = response.getCode().toString();
		if (!code.startsWith("2")) {
			System.out.println("Error: " + code);
			return;
		}

        String responseText = response.getResponseText();

        //Creeate la classe sensor
    }

    private static void observe(Sensor s) {
		CoapCollector observer = new CoapCollector(s);
		observer.startObserving();
	}
}
