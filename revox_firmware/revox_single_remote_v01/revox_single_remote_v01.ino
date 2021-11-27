/*
 * (C) 2019 Jonathan Reus
 * Single Revox controller with an Arduino connected to a
 * UDN2981 high-side driver array. 
 */

#define STOP 2
#define PLAY 3
#define PAUSE 4
#define REC 5
#define FWD 6
#define REW 7

void setup() {
  pinMode(STOP, OUTPUT);
  pinMode(PLAY, OUTPUT);
  pinMode(PAUSE, OUTPUT);
  pinMode(REC, OUTPUT);
  pinMode(FWD, OUTPUT);
  pinMode(REW, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(STOP, LOW);
  digitalWrite(PLAY, LOW);
  digitalWrite(PAUSE, LOW);
  digitalWrite(REC, LOW);
  digitalWrite(FWD, LOW);
  digitalWrite(REW, LOW);
  digitalWrite(13, LOW);

  Serial.begin(9600);
}

void loop() {

}
