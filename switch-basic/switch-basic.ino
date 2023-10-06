void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
  pinMode(17, OUTPUT);
  pinMode(27, OUTPUT);
}

void loop() {
  int sensorVal = digitalRead(16);
  if(sensorVal == 1) {
    Serial.println("if");
    digitalWrite(17, 1);
    digitalWrite(27, 0);
  }else {
    Serial.println("sensorVal");
    digitalWrite(27, 1);
    digitalWrite(17, 0);
  }
}
