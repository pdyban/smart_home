// WIFI settings
#define wifi_ssid             "my_wifi_ssid"
#define wifi_password         "my_wifi_password"
#define wifi_device_static_ip IPAddress(192,168,0,my_device_static_ip)
#define wifi_gateway          IPAddress(192,168,0,my_router_static_ip)
#define wifi_subnet_mask      IPAddress(255,255,255,0)

// MQQT settings
#define mqtt_server      "my_mqtt_broker_ip"
#define mqtt_user        "my_mqtt_broker_username"
#define mqtt_password    "my_mqtt_broker_password"
#define mqtt_client_name "my_mqtt_broker_name"
#define mqtt_server_port my_mqtt_broker_port

#define humidity_topic    "sensor/humidity"
#define temperature_topic "sensor/temperature"
#define pressure_topic    "sensor/pressure"
#define pm10_topic        "sensor/pm10"
#define pm25_topic        "sensor/pm25"
#define debug_topic       "sensor/debug"

// delay between alive cycles in ms
#define refresh_delay      30000 // previous value: 10000
// delay after publishing MQTT messages in ms
#define mqtt_publish_delay 150
