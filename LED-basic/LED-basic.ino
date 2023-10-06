const int Led_1 = 16;
const int Led_2 = 17;
const int Led_3 = 27;

void setup() {
  pinMode(Led_1, OUTPUT);
  pinMode(Led_2, OUTPUT);
  pinMode(Led_3, OUTPUT);
}

void loop() {
  digitalWrite(Led_1, 1);
  delay(1000);
  digitalWrite(Led_1, 0);
  delay(1000);

  digitalWrite(Led_2, 1);
  delay(1000);
  digitalWrite(Led_2, 0);
  delay(1000);

  digitalWrite(Led_3, 1);
  delay(1000);
  digitalWrite(Led_3, 0);
  delay(1000);
}
