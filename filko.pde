const int SELECTOR_PIN = A0;
const int POWER_PIN = 12;
const int SW2_PIN = 9;
const int SW1_PIN = 11;
const int METER_PIN = 10;

void setup() {
  pinMode(SELECTOR_PIN, INPUT);
  digitalWrite(SELECTOR_PIN, HIGH);
  pinMode(POWER_PIN, INPUT);
  digitalWrite(POWER_PIN, HIGH);
  pinMode(SW1_PIN, INPUT);
  digitalWrite(SW1_PIN, HIGH);
  pinMode(SW2_PIN, INPUT);
  digitalWrite(SW2_PIN, HIGH);
  pinMode(METER_PIN, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  int selection;
  char outStr[80];
  int selectorVal = analogRead(SELECTOR_PIN);

  snprintf(outStr, 80, "0,%d,%d,%d,%d", 
    selection,
    selectorVal,
    digitalRead(POWER_PIN),
    digitalRead(SW1_PIN),
    digitalRead(SW2_PIN));

  Serial.println(outStr);
  delay(100);
}
