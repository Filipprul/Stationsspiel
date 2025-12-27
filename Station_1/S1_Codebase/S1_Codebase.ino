void setup() {
}

void callback(char* topic, byte* payload, unsigned int length){
  String message = "";

  for (int i = 0; i < length; i++){
    message += (char)payload[i];
  }

  if(message == "START"){
    digitalWrite(LED_START, HIGH);
  } else-if(message == "SOLVED"){
    digitalWrite(LED_SOLVED, HIGH);
  } else-if(message == "RESET"){
    ESP.restart();
    message = START;
  }
}

void loop() {
}
