// WIFI settings
#define wifi_ssid "my_wifi_ssid"
#define wifi_password "my_wifi_password"

// MQQT settings
#define mqtt_server "my_mqtt_broker_ip"
#define mqtt_user "my_mqtt_broker_username"
#define mqtt_password "my_mqtt_broker_password"
#define mqtt_client_name "my_mqtt_broker_name"
#define mqtt_server_port my_mqtt_broker_port

#define humidity_topic "sensor/humidity"
#define temperature_topic "sensor/temperature"
#define pm10_topic "sensor/pm10"
#define pm25_topic "sensor/pm25"
#define debug_topic "sensor/debug"

// delay between alive cycles in ms
#define refresh_delay 5000
// delay after wifi (dis-)connect functions in ms
#define wifi_delay 1000
// delay after publishing MQTT messages in ms
#define mqtt_publish_delay 50
