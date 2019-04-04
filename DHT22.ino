#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN D2
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

const char* ssid = "WifiSSid";
const char* password =  "Wifipass";
const char* mqttServer = "ServerAdress";
const int mqttPort = 13017;
const char* mqttUser = "user";
const char* mqttPassword = "pass";
uint32_t delayMS;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }

  //client.publish("esp", "Hello from ESP8266");
  //client.subscribe("esp/test");
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}

void loop() {
  //Events
  sensors_event_t Cevent;
  sensors_event_t Hevent;
  //MQTT Client Loop
  client.loop();

  //Temperature -- Humidity
  dht.temperature().getEvent(&Cevent);
  dht.humidity().getEvent(&Hevent);
  if (isnan(Cevent.temperature) && isnan(Hevent.relative_humidity)) {
    //Serial.println("Gelen Yok");
  }
  else {
    String message = "{\"C\":";
    message += Cevent.temperature;
    message += ",\"H\":";
    message += Hevent.relative_humidity;
    message += "}";
    //Serial.println(message);
    client.publish("test", (char*) message.c_str());
  }
  delay(5000);

}
