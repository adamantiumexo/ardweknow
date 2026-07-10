void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(7, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  int value = analogRead(A0);
  if (value <= 1000){ // 0-1024 512 256 5V 
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Low");
  }
  else{
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print("High");
  }
  delay(500);
}
