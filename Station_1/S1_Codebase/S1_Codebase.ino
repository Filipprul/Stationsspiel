#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your_SSID"; // Ans WLAN anspassen
const char* password = "your_PASSWORD"; // Ans WLAN anspassen
const char* mqtt_server = "your_IP"; // Lokale IP-Adresse

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;

const int senderPins[] = {0, 1, 2, 3};
const int empfangPins[] = {4, 5, 6, 7};
bool status = false;

const int contPin = 8;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  for(int i = 0; i < 4; i++){
    pinMode(senderPins[i], OUTPUT);
    digitalWrite(senderPins[i], LOW);
    pinMode(empfangPins[i], INPUT_PULLDOWN);
  }
  Serial.println("System bereit!");
}

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Verbindet mit WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("...");
    }

    Serial.println("");
    Serial.println("WiFi verbunden");
    Serial.println("IP addresse: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Nachricht angekommen. Topic: ");
    Serial.print(topic);
    Serial.print(". Nachricht: ");
    String messageTemp;

    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    if (String(topic) == "SF/Sub") {
        Serial.print("Ändert status zu: ");
        if (messageTemp == "on") {
            Serial.println("An");
            digitalWrite(contPin, HIGH);
        } else if (messageTemp == "off") {
            Serial.println("Aus");
            digitalWrite(contPin, LOW);
        }
    }
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Versuche MQTT Verbindung...");
        String clientId = "ESP32-" + WiFi.macAddress();
        if (client.connect(clientId.c_str())) {
            Serial.println("Verbunden!");
            client.subscribe("SF/Sub"); // MQTT-Kommunikation
        } else {
            Serial.print("Fehler, rc=");
            Serial.print(client.state());
            Serial.println(" Versuche es in 5 Sekunden erneut.");
            delay(5000);
        }
    }
}

void check(){
  if (status) return;

  int korrektePins = 0;
  for(int i = 0; i < 4; i++){
    digitalWrite(senderPins[i], HIGH);
    delay(5);
    if (digitalRead(empfangPins[i]) == HIGH){
      korrektePins++;
    }
    digitalWrite(senderPins[i], LOW);
  }

  if (korrektePins == 4){
    status = true;
    client.publish("SF/station1/status", "geloest"); // MQTT-Nachricht
    Serial.println("Rätsel gelöst!");
  }
}

void loop(){
  if (!client.connected()) {
      reconnect();
  }
  client.loop();
  check();
  delay(100);
}
