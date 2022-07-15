# IoT_project

Project developed for the Internet of Things course at the University of Pisa

## Repository
The repository is organized as follows:
- *simulation/* contains simulation scenarios to be imported in Cooja
- *rpl-border-router/* contains the source code of the Contiki-NG border router
- *collector/* contains the source code of the Java collector
- *nodes/* contains the source code of both MQTT and CoAP nodes implementation and the modules simulating their sensors
- *docs/* contains the report
- *SmartGenerator-simulation.csc* Cooja simulation
- *SmartGenerators-grafana.json* Grafan dashboard

## Requirements
- Eclipse Mosquitto MQTT broker
- MySQL database
- Contiki NG - https://github.com/contiki-ng/contiki-ng/ with Cooja simulator to run the simulation
- Grafana

## Run the project

- Download the project into the *contiki-ng/examples* folder
- Import the grafana dashboard *SmartGenerators-grafana.json*
- Run MySql database and MQTT broker
 ### Database
- Create the database with *CREATE DATABASE smartgenerator;*
- Import *smartgenerators_dump.sql*
 ### Collector
- In the collector folder, compile the collector : *mvn clean install package*
- and run the application : *java -jar target/collector.iot.unipi.it-0.0.1-SNAPSHOT.jar*

### Simulation
- Import *SmartGenerator-simulation.csc* in Cooja
- Add the socket on the border router : Tools -> Serial Socket (SERVER) -> Contiki 1 – Press START
- Use the tunslip6: make TARGET=cooja connect-router-cooja

### Deployment on the nRF52840-Dongle
Flash the code on all the sensors:
- Border rouder: make TARGET=nrf52840 BOARD=dongle border-router.dfu-upload PORT=/dev/ttyACM0
- MQTT nodes: make TARGET=nrf52840 BOARD=dongle mqtt-sensor.dfu-upload PORT=/dev/ttyACM0
- CoAP nodes: make TARGET=nrf52840 BOARD=dongle coap-sensor.dfu-upload PORT=/dev/ttyACM0

Powers all sensors and use tunslip6 to connect with the border router
- make TARGET=nrf52840 BOARD=dongle connect-router PORT=/dev/ttyACM0
