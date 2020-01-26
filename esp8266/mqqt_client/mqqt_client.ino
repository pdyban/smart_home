/*
  Author: Pavlo Dyban
  Email: pdyban@gmail.com
  License: MIT
*/

// Select modules that you want to compile:
//#define USING_SDS011
#define USING_BME280
//#define USING_DHT22

// MQQT includes
#include "EspMQTTClient.h"

// SDS011 includes
#ifdef USING_SDS011
  #include "SdsDustSensor.h"
#endif // USING_SDS011

// DHT22 includes
#ifdef USING_DHT22
  #include <DHT.h>
#endif // USING_DHT22

// BME280 includes
#ifdef USING_BME280
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
#endif // USING_BME280

// Wifi includes
extern "C" {
  #include "user_interface.h"  // Required for wifi_station_connect() to work
}

// Configuration
#include "smart_home_config.h"

// MQTT configuration
EspMQTTClient client(
  wifi_ssid,
  wifi_password,
  mqtt_server,      // MQTT Broker server ip
  mqtt_user,        // Can be omitted if not needed
  mqtt_password,    // Can be omitted if not needed
  mqtt_client_name, // Client name that uniquely identifies your device
  mqtt_server_port  // The MQTT port, default to 1883
);

// SDS011 configuration
#ifdef USING_SDS011
  int rxPin = D1; // rxPin has to be connected to TXD on SDS011 board
  int txPin = D2; // txPin has to be connected to RXD on SDS011 board
  SdsDustSensor sds(rxPin, txPin);
#endif // USING_SDS011

// DHT22 configuration
#ifdef USING_DHT22
  #define DHTPIN D7     // what digital pin the DHT22 is conected to
  #define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
  DHT dht(DHTPIN, DHTTYPE);
#endif // USING_DHT22

// BME280 configuration
#ifdef USING_BME280
  Adafruit_BME280 bme; 
  #define SEALEVELPRESSURE_HPA (1013.25)
#endif // USING_BME280

// Wifi configuration
#define FPM_SLEEP_MAX_TIME 0xFFFFFFF

void enableWifi() {
  wifi_fpm_do_wakeup();
  wifi_fpm_close();

  Serial.println("Enabling Wifi chip...");
  wifi_set_opmode(STATION_MODE);
  wifi_station_connect();
  
  delay(wifi_delay);
}

void disableWifi() {
  Serial.println("Disabling Wifi chip...");
  
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_set_sleep_type(MODEM_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);
  delay(wifi_delay);
}

void setup()
{
  Serial.println("setup()");
  Serial.begin(115200);

  //setupLED();
  //enableLED();

  setupMQTTClient();
  
#ifdef USING_SDS011
  setupSDS011();
#endif // USING_SDS011

#ifdef USING_DHT22
  setupDHT22();
#endif // USING_DHT22
  
#ifdef USING_BME280
  setupBME280();
#endif // USING_BME280
}

void setupMQTTClient() {  
  enableWifi();
  delay(wifi_delay);
  
  Serial.println("setupMQTTClient()");
  // Optional functionalities of EspMQTTClient 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage(debug_topic, "Client disconnected", true);  // You can activate the retain flag by setting the third parameter to true
}


//////////////////////////////////////////////////////
// LED
//////////////////////////////////////////////////////
void setupLED() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
}

void enableLED() {
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
}

void disableLED() {
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
}

//////////////////////////////////////////////////////
// SDS 011
//////////////////////////////////////////////////////
#ifdef USING_SDS011
void setupSDS011() {
  Serial.println("setupSDS011()");
  sds.begin();

  sds.wakeup();

  Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setContinuousWorkingPeriod().toString()); // ensures sensor has continuous working period - default but not recommended 
}

void publishSDS011() {
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
}

void disableSDS011() {
  Serial.println("Disabling SDS011...");
  WorkingStateResult state = sds.sleep();
  if (state.isWorking()) {
    Serial.println("Problem with putting the sensor to sleep.");
  }
  else {
    Serial.println("Successfully put sensor to sleep");
  }
}
#endif // USING_SDS011

//////////////////////////////////////////////////////
// DHT 22
//////////////////////////////////////////////////////
#ifdef USING_DHT22
void setupDHT22() {
  Serial.println("setupDHT22()");
  dht.begin();
}

void publishDHT22() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (DHT22 is a slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  client.publish(temperature_topic, String(t).c_str());
  client.publish(humidity_topic, String(h).c_str());    

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.println(" *C ");
}
#endif // USING_DHT22

//////////////////////////////////////////////////////
// BME 280
//////////////////////////////////////////////////////
#ifdef USING_BME280
void setupBME280() {
  Serial.println("setupBME280()");
  if(!bme.begin(0x76)) {
    Serial.println("Could not establish connection to BME280");
  }
  else {
    Serial.println("Successfully established connection to BME280");
  }
}

void publishBME280() {
  Serial.println("publishBME280()");
  
  float t, h, p, a;
  t = bme.readTemperature();
  h = bme.readHumidity();
  p = bme.readPressure() / 100.0F;
  a = bme.readAltitude(SEALEVELPRESSURE_HPA);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Pressure: ");
  Serial.print(p);
  Serial.print(" Pa ");
  Serial.print("Altitude: ");
  Serial.print(a);
  Serial.println(" m");

  client.publish(temperature_topic, String(t).c_str());
  client.publish(humidity_topic, String(h).c_str());   
  client.publish(pressure_topic, String(p).c_str());    
}
#endif // USING_BME280

// This function is called once everything is connected (Wifi and MQTT)
void onConnectionEstablished()
{
  client.publish(debug_topic, "Connected!"); // You can activate the retain flag by setting the third parameter to true
  Serial.println("Connection with Wifi network established.");
}

void loop()
{
  client.loop();
  Serial.println("loop()");

  if(!client.isConnected()) { 
    if(!client.isWifiConnected()) {
      Serial.println("Could not establish Wifi connection.");
    }
    else if(!client.isMqttConnected()) {
      Serial.println("Could not establish MQTT connection.");
    }
  }
  else {
#ifdef USING_SDS011
    publishSDS011();
#endif // USING_SDS011

#ifdef USING_DHT22
    publishDHT22();
#endif // USING_DHT22
  
#ifdef USING_BME280
    publishBME280();
#endif // USING_BME280

    delay(mqtt_publish_delay);
  }

#ifdef USING_SDS011
  disableSDS011();
#endif // USING_SDS011

  disableWifi();

  //disableLED();
  
  Serial.print("Going into deep sleep for ");
  Serial.print(refresh_delay/1000);
  Serial.println("s");
  ESP.deepSleep(refresh_delay*1e3, WAKE_RF_DEFAULT);
}
