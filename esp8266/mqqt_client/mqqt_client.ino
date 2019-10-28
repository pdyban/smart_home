/*
  TODO: https://gist.github.com/balloob/1176b6d87c2816bd07919ce6e29a19e9
*/

// MQQT includes
#include "EspMQTTClient.h"

// SDS011 includes
#include "SdsDustSensor.h"

// DHT22 includes
#include <DHT.h>

#include "smart_home_config.h"

// MQTT configuration
EspMQTTClient client(
  wifi_ssid,
  wifi_password,
  mqtt_server,      // MQTT Broker server ip
  mqtt_user,        // Can be omitted if not needed
  mqtt_password,    // Can be omitted if not needed
  mqtt_client_name, // Client name that uniquely identify your device
  mqtt_server_port  // The MQTT port, default to 1883. this line can be omitted
);

// SDS011 configuration
int rxPin = D1; // rxPin has to be connected to TXD on SDS011 board
int txPin = D2; // txPin has to be connected to RXD on SDS011 board
SdsDustSensor sds(rxPin, txPin);

// DHT22 configuration
float temp = 0.0f;

#define DHTPIN D7     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
DHT dht(DHTPIN, DHTTYPE);
int dht22_timeSinceLastRead = 0;

void setup()
{
  Serial.begin(115200);

  setupMQTTClient();

  setupSDS011();

  setupDHT22();
}

void setupMQTTClient() {  
  // Optional functionalities of EspMQTTClient 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  //client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

void setupSDS011() {
  sds.begin();

  Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setContinuousWorkingPeriod().toString()); // ensures sensor has continuous working period - default but not recommended 
}

void setupDHT22() {
  dht.begin();
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Publish a message
  //client.publish(temperature_topic, String(0.0).c_str()); // You can activate the retain flag by setting the third parameter to true
  Serial.println("Connection with Wifi network established.");
}

void loop()
{
  client.loop();

  if(!client.isConnected()) { 
    Serial.println("Wifi connection is not established");
    delay(300000); // 5 mins delay
  } 
  else {
    publishSDS011();
  
    publishDHT22();
  }

  delay(refresh_delay);
  dht22_timeSinceLastRead += refresh_delay;
}

void publishSDS011() {
  sds.wakeup();
  //delay(10000); // time during which PM measurements are accumulated for higher accuracy
  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    client.publish(pm25_topic, String(pm.pm25).c_str());
    client.publish(pm10_topic, String(pm.pm10).c_str());
  } else {
    // notice that there is a loop delay and some reads are not available
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }

//  WorkingStateResult state = sds.sleep();
//  if (state.isWorking()) {
//    Serial.println("Problem with putting the sensor to sleep.");
//  }
//  else {
//    Serial.println("Successfully put sensor to sleep");
//  }
}


void publishDHT22() {
  // Report every 2 seconds.
  //Serial.println(dht.getMinimumSamplingPeriod());
  //Serial.println(dht.getStatusString());
  if(dht22_timeSinceLastRead > 2000) {
    dht22_timeSinceLastRead = 0;
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    // float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
//    Serial.print(f);
//    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
//    Serial.print(hif);
//    Serial.println(" *F");
  
    client.publish(temperature_topic, String(t).c_str());
    client.publish(humidity_topic, String(h).c_str());    
  }
}
