package it.unipi.dii.iot.smartgenerator.coap;

import java.net.InetAddress;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.server.resources.CoapExchange;
import it.unipi.dii.iot.smartgenerator.utils.Sensor;

/**
 * Implements the CoapRegistration resource.
 */
public class CoapRegistration extends CoapResource{

    public CoapRegistration(String name) {
        super(name);
    }
    
    public void handleGET(CoapExchange exchange){
         
        exchange.accept();
        InetAddress sensorIp = exchange.getSourceAddress();
        System.out.println("Registration request from: " +sensorIp);

        Sensor temperatureSensor = new Sensor(sensorIp.getHostAddress(), "temperature");
        observe(temperatureSensor);
        Sensor energySensor = new Sensor(sensorIp.getHostAddress(), "energy_generated");
        observe(energySensor);
        Sensor fuelSensor = new Sensor(sensorIp.getHostAddress(), "fuel_level");
        observe(fuelSensor);
    }

    private static void observe(Sensor s) {
		CoapCollector observer = new CoapCollector(s);
        System.out.println("CoapCollector created for " + s.getResourcePath());
		observer.startObserving();
	}
}
