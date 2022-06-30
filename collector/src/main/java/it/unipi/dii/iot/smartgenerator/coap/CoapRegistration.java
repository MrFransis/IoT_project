package it.unipi.dii.iot.smartgenerator.coap;

import java.net.InetAddress;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.coap.CoAP.ResponseCode;
import org.eclipse.californium.core.server.resources.CoapExchange;

import it.unipi.dii.iot.smartgenerator.utils.Sensor;

public class CoapRegistration extends CoapResource{

    public CoapRegistration(String name) {
        super(name);
    }
    
    public void handleGET(CoapExchange exchange){
         
        exchange.accept();
        InetAddress sensorIp = exchange.getSourceAddress();
        System.out.println(sensorIp);
        //response?

        Sensor sensor = new Sensor(sensorIp.getHostAddress(), "coolant");
        observe(sensor);
    }

    private static void observe(Sensor s) {
		CoapCollector observer = new CoapCollector(s);
		observer.startObserving();
	}
}
