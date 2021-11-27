/*
 * (C) 2019 Jonathan Reus
 * 
 * v2 Revox Remote Control Firmware
 * Basic Control using a MCP23017 GPIO expander of 
 * a series of UDN2981 driver switches.
 * 
 * All about the MCP23017 library 
 * https://www.best-microcontroller-projects.com/mcp23017.html
 * https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library
 */
#include <Wire.h>
#include <MCP23017.h>

// GPIO Output Pins
#define STOP 0
#define PLAY 1
#define PAUSE 2
#define REC 3
#define FWD 4
#define REW 5


#define MCP23017_ADDR_A 0x20
#define MCP23017_ADDR_B 0x27
MCP23017 mcpA = MCP23017(MCP23017_ADDR_A);
MCP23017 mcpB = MCP23017(MCP23017_ADDR_B);




void setup() {
  Wire.begin();
  mcpA.init();
  mcpB.init();
  //Serial.begin(115200);

  // Set all GPIO expansion pins as outputs and write low.
  for(int i = 0; i < 16; i++) {
    mcpA.pinMode(i, OUTPUT);
    mcpA.digitalWrite(i, LOW);
  }
  for(int i = 0; i < 16; i++) {
    mcpB.pinMode(i, OUTPUT);
    mcpB.digitalWrite(i, LOW);
  }
  
}

void loop() {
  // RECEIVE A COMMAND FROM PC TO TRIGGER SOMETHING ON THE REVOX MACHINE
  int playToRewRatio = 12000 / 3800;


// PLAY
mcpA.digitalWrite(PLAY, HIGH);
delay(100);
mcpA.digitalWrite(PLAY, LOW);
delay(12000);
mcpA.digitalWrite(STOP, HIGH);
delay(100);
mcpA.digitalWrite(STOP, LOW);
delay(3000);

// REWIND
mcpA.digitalWrite(REW, HIGH);
delay(100);
mcpA.digitalWrite(REW, LOW);
delay(12000 / playToRewRatio);
mcpA.digitalWrite(STOP, HIGH);
delay(100);
mcpA.digitalWrite(STOP, LOW);
delay(3000);


// RECORD
mcpA.digitalWrite(REC, HIGH);
mcpA.digitalWrite(PLAY, HIGH);
delay(100);
mcpA.digitalWrite(REC, LOW);
mcpA.digitalWrite(PLAY, LOW);
delay(12000);

mcpA.digitalWrite(STOP, HIGH);
delay(100);
mcpA.digitalWrite(STOP, LOW);
delay(3000);

// REWIND
mcpA.digitalWrite(REW, HIGH);
delay(100);
mcpA.digitalWrite(REW, LOW);
delay(12000 / playToRewRatio );
mcpA.digitalWrite(STOP, HIGH);
delay(100);
mcpA.digitalWrite(STOP, LOW);
delay(3000);


 
// end delay
delay(1000);
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
char dataCode, inByte;
int dataval;
void serialEvent() {
  while (Serial.available()) {
    inByte = (char)Serial.read();
    if(inByte == '\n') { // read new datapoint startbyte
      dataCode = (char)Serial.read();
      dataVal = Serial.parseInt();
    }
    // Do something with the data?
    Serial.print("RECEIVED: ");
    Serial.print(dataCode);
    Serial.print("  value: ");
    Serial.println(dataVal);  
  }
}
