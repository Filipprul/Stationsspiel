#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your_SSID"; // Ans WLAN anspassen
const char* password = "your_PASSWORD"; // Ans WLAN anspassen
const char* mqtt_server = "your_IP"; // Lokale IP-Adresse

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;

#define ButtonPin 2
bool status = false;
unsigned long startTime = 0.0;
unsigned long endTime = 0.0;
int flag = 0;
bool lastButtonState = LOW;

const int contPin = 8;

void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    pinMode(ButtonPin, INPUT_PULLDOWN);
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


void check() {
    if (status) return;

    bool currentButtonState = digitalRead(ButtonPin);

    if (currentButtonState == HIGH && lastButtonState == LOW) {
        delay(50); // Entprellen
        if (flag == 0) {
            startTime = millis();
            flag = 1;
            Serial.println("Zeitmessung gestartet...");
        }
        else if (flag == 1) {
            endTime = millis();
            flag = 2;
            Serial.println("Zeitmessung gestoppt.");
        }
    }
    lastButtonState = currentButtonState;

    if (flag == 2) {
        float diff = (endTime - startTime) / 1000.0;
        Serial.print("Zeit: ");
        Serial.print(diff);
        Serial.println(" s.");

        if (diff >= 27.0 && diff <= 33.0) {
            Serial.println("Rätsel gelöst!");
            client.publish("SF/station1/status", "geloest");
            status = true;
        } else {
            Serial.println("Zu ungenau! Versuch es nochmal.");
            flag = 0;
            status = false;
        }
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