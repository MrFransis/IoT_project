package it.unipi.dii.iot.smartgenerator.coap;

import java.net.InetAddress;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP.ResponseCode;
import org.eclipse.californium.core.server.resources.CoapExchange;
import it.unipi.dii.iot.smartgenerator.utils.Sensor;

/**
 * Implements the CoapRegistration resource.
 */
public class CoapRegistration extends CoapResource{

    static Map<String, HashSet<String>> registeredNodes;

    public CoapRegistration(String name) {
        super(name);
        registeredNodes = new HashMap<>();
        registeredNodes.put("temperature", new HashSet<>());
        registeredNodes.put("fuel_level", new HashSet<>());
        registeredNodes.put("energy_generated", new HashSet<>());
    }
    
    public void handleGET(CoapExchange exchange){
         
        exchange.accept();
        InetAddress sensorIp = exchange.getSourceAddress();
        System.out.println("Registration request from: " + sensorIp);
        String hostAddress = sensorIp.getHostAddress();

        if (!registeredNodes.get("temperature").contains(hostAddress)) {
            registeredNodes.get("temperature").add(hostAddress);
            Sensor temperatureSensor = new Sensor(hostAddress, "temperature");
            observe(temperatureSensor);
        }

        if (!registeredNodes.get("energy_generated").contains(hostAddress)) {
            registeredNodes.get("energy_generated").add(hostAddress);
            Sensor energySensor = new Sensor(hostAddress, "energy_generated");
            observe(energySensor);
        }

        if (!registeredNodes.get("fuel_level").contains(hostAddress)) {
            registeredNodes.get("fuel_level").add(hostAddress);
            Sensor fuelSensor = new Sensor(hostAddress, "fuel_level");
            observe(fuelSensor);
        }
    }

    private static void observe(Sensor s) {
		CoapCollector observer = new CoapCollector(s);
        System.out.println("CoapCollector created for " + s.getResourcePath());
		observer.startObserving(registeredNodes);
	}
}
