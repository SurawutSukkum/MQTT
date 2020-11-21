/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define RELAY_1 14
#define DHTPIN 2 
#define DHTTYPE    DHT11     // DHT 11


DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "HOST";
const char* password = "PASSWORD";
const char* mqtt_server = "192.168.137.87";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
   Serial.println((char)payload[0]);
  if ((char)payload[0] == '0') {
    digitalWrite(RELAY_1, LOW);   
  } else {
    digitalWrite(RELAY_1, HIGH);  
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "clientId-surawut";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("clientId-MaPfWMOvoQ","mymqtt","12345678")) {
      client.publish("outTopic", "hello world");
      client.subscribe("inTopic");
    } else {
      delay(5000);
    }
  }
}

void setup() {

    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    dht.begin();
    pinMode(RELAY_1, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
}

void loop() {

    char h = dht.readHumidity();
    char t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    snprintf (msg, 52, "temp,%1d,",char(t-10));
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic/", msg);
    delay(100);
}
