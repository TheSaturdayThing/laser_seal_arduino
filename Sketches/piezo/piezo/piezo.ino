int photosensorPin = 0;
int piezoPin = 9;

int val = 0;

void setup() {
  pinMode(piezoPin, OUTPUT);
}

void loop() {
  digitalWrite(piezoPin, LOW);
  val = analogRead(photosensorPin);
  val = val/2;

  for( int i=0; i// play it for 50 cycles
    digitalWrite(piezoPin, HIGH);
    delayMicroseconds(val);
    digitalWrite(piezoPin, LOW);
    delayMicroseconds(val);
  }
}
