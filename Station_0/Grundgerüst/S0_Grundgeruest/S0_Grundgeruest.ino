// Ermöglicht die verbindung mit einem WiFi-Netzwerk
#include <WiFi.h>

// Ermöglicht die Kommunikation mit einem MQTT-Broker
#include <PubSubClient.h>

// SSID des WiFi-Netzwerks
const char* ssid = "your_SSID";

// Passwort des WiFi-Netzwerks
const char* password = "your_PASSWORD";

// Der Broker über den die Nachrichten gesendet werden
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;

const int ledPin = 2;

void setup() {
    Serial.begin(115200);
    setup_wifi();   // Verbindung mit dem WiFi-Netzwerk herstellen
    client.setServer(mqtt_server, 1883);    // MQTT-Broker einstellen
    client.setCallback(callback);   // Callback-Funktion einstellen

    pinMode(ledPin, OUTPUT);
}

void setup_wifi() {
    // Der ESP32 versucht sich mit dem WiFi-Netzwerk zu verbinden und gibt im
    // Monitor Punkte aus, bis die Verbindung steht.
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
    // Sobald eine Nachricht an SF/LED gesendet wird, prüft die If-Anweisung und
    // führt die jeweilige Aktion aus.
    Serial.print("Nachricht angekommen. Topic: ");
    Serial.print(topic);
    Serial.print(". Nachricht: ");
    String messageTemp;

    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    if (String(topic) == "SF/LED") {
        Serial.print("Ändert status zu: ");
        if (messageTemp == "on") {
            Serial.println("An");
            digitalWrite(ledPin, HIGH);
        } else if (messageTemp == "off") {
            Serial.println("Aus");
            digitalWrite(ledPin, LOW);
        }
    }
}

void reconnect() {
    // Falls die Verbindung zum Broker verloren geht oder abgebrochen wird,
    // versucht der ESP32 sich erneut zu verbinden und abonniert das Topic
    // SF/LED damit keine Befehle verpasst werden.
    while (!client.connected()) {
        Serial.print("Versuche MQTT Verbindung...");
        String clientId = "ESP32-" + WiFi.macAddress();
        if (client.connect(clientId.c_str())) {
            Serial.println("Verbunden!");
            client.subscribe("SF/LED");
        } else {
            Serial.print("Fehler, rc=");
            Serial.print(client.state());
            Serial.println(" Versuche es in 5 Sekunden erneut.");
            delay(5000);
        }
    }
}

void loop() {
    // Stellt sicher, dass der ESP32 mit dem Broker verbunden ist
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}