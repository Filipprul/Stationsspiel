const int kabelPins[] = {1, 2, 3, 4};
bool status = false;

void setup() {
  Serial.begin(115200);

  for(i = 0; i < 4; i++){
    pinMode(kabelPins[i], INPUT_PULLUP);
  }
  Serial.println("System bereit!");
}

void check(){
  if (status) return;

  bool pinsdrin = false;
  for(int i = 0; i < 4; i++){
    if (digitalRead(kabelPins[i]) == HIGH){
      pinsdrin = true;
    }
  }

  if (pinsdrin == false){
    status = true;
    Serial.println("Rätsel gelöst!")
  }
}

void loop(){
  check();
  delay(100);
}
