#define PLAY_PIN 2
#define STOP_PIN 3

void setup() {
  // put your setup code here, to run once:
  pinMode(PLAY_PIN, OUTPUT);
  pinMode(STOP_PIN, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(PLAY_PIN, HIGH);
  digitalWrite(STOP_PIN, HIGH);

  Serial.begin(9600);

}

void loop() {


  // MOSFET CONTROL, AN OPEN MOSFET SHOWS THE OUTPUT GROUND
  digitalWrite(PLAY_PIN, LOW); // turn off mosfet, show 24V to the output
  digitalWrite(13, HIGH);
  delay(300); // just a quick pulse is enough to trigger it
  digitalWrite(PLAY_PIN, HIGH); // turn on mosfet, show GND to the output
  digitalWrite(13, LOW);
  Serial.println("PLAY");

  delay(8000); // play for 8 seconds, then stop

  digitalWrite(STOP_PIN, LOW);
  digitalWrite(13, HIGH);
  delay(300);
  digitalWrite(STOP_PIN, HIGH);
  digitalWrite(13, LOW);
  Serial.println("STOP");

  // Wait 5 seconds
  delay(5000);

  


/*
  // PLAY
  digitalWrite(PLAY_PIN, LOW); // pin LOW shows 24V to the remote trigger
  digitalWrite(13, HIGH);
  delay(4000); // trigger needs to be about X ms
  digitalWrite(PLAY_PIN, HIGH);
  digitalWrite(13, LOW);
  
  delay(8000);


  // STOP
  
  digitalWrite(STOP_PIN, HIGH);
  delay(100);
  digitalWrite(STOP_PIN, LOW);
  digitalWrite(13, LOW);
  delay(5000); // Wait for 5 seconds...
  */

}
