/*
 * (C) 2020 Jonathan Reus
 * 
 * v4 Revox Remote Control Firmware controls 1x GPIO boards
 * Serial input commands to send messages to the tape machine.
 */
#include <Wire.h>
#include <MCP23017.h>

// GPIO Expander Output Pin Numbering (ordering matches Revox remote pins)
#define STOP 4
#define PLAY 3
#define PAUSE 0
#define REC 5
#define FWD 2
#define REW 1
#define CAP 6 // capstan speed control inhibit (float the capstan speed control)


// Capstan control voltage output, goes through a LOPASS filter and to an opamp with GAIN=2.5
#define CAPCV_A 3
#define CAPCV_B 5
//#define CAPCV_C 6
//#define CAPCV_D 9


#define MCP23017_ADDR_A 0x20
//#define MCP23017_ADDR_B 0x27
MCP23017 mcpA = MCP23017(MCP23017_ADDR_A);
//MCP23017 mcpB = MCP23017(MCP23017_ADDR_B);




void setup() {
  Wire.begin();
  mcpA.init();
  //mcpB.init();
  Serial.begin(115200);
  Serial.println("Hello");

  // Set all GPIO expansion pins as outputs and write low.
  for(int i = 0; i < 16; i++) {
    mcpA.pinMode(i, OUTPUT);
    mcpA.digitalWrite(i, LOW);
  }
  /*
  for(int i = 0; i < 16; i++) {
    mcpB.pinMode(i, OUTPUT);
    mcpB.digitalWrite(i, LOW);
  }
  */
  
}

void loop() { 
  delay(15); // delay for stability
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
byte startCode[] = {244, 253}; // must match startcode in SuperCollider
char dataCode;
byte dataByte1, dataByte2;
unsigned int dataVal;

void serialEvent() {
  byte inByte1, inByte2;
  byte dataCode, cmdCode, extraByteHigh, extraByteLow;
  unsigned int extra;
  while (Serial.available()) {
    inByte1 = Serial.read();
    inByte2 = Serial.read();
    if(inByte1 == startCode[0] && inByte2 == startCode[1]) { // read new datapoint startbyte
      dataCode = (char)Serial.read();
      cmdCode = Serial.read();
      extraByteHigh = Serial.read();
      extraByteLow = Serial.read();
      extra = (extraByteHigh << 8) + extraByteLow;
      
      sendCommand(dataCode - 65, cmdCode, extra);
      

    }

  }
}

#define TRIG_MS 10

void sendCommand(byte machine_id, byte cmdCode, unsigned int extra) {
  MCP23017 * mcp;
  byte offset=0, capstan_cv_pin=CAPCV_A;
  switch(machine_id) {
    case 0:
      mcp = &mcpA; offset=0;
      capstan_cv_pin = CAPCV_A;
    break;
    case 1:
      mcp = &mcpA; offset=8;
      capstan_cv_pin = CAPCV_B;
    break;
    default:  //Invalid device id, just return
      return;
  }

  // Command Map (must match command map in SuperCollider)
  //byte cmds[] = {/*STOP*/3, /*PLAY*/9, /*REC*/19, /*PAUSE*/22, /*REW*/33, /*FF*/41, 
  //  /*CAPSTAN_ENABLE*/27, /*CAPSTAN_ADJUST*/43 };
  switch(cmdCode) {
    case 3: //STOP
      mcp->digitalWrite(STOP+offset, HIGH);
      delay(TRIG_MS);
      mcp->digitalWrite(STOP+offset, LOW);
    break;
    case 9: //PLAY
      mcp->digitalWrite(PLAY+offset, HIGH);
      delay(TRIG_MS);
      mcp->digitalWrite(PLAY+offset, LOW);
    break;
    case 19: //REC
      mcp->digitalWrite(REC+offset, HIGH);
      mcp->digitalWrite(PLAY+offset, HIGH);
      delay(TRIG_MS);
      mcp->digitalWrite(REC+offset, LOW);
      mcp->digitalWrite(PLAY+offset, LOW);
    break;
    case 22: //PAUSE
      mcp->digitalWrite(PAUSE+offset, HIGH);
      delay(TRIG_MS + extra);
      mcp->digitalWrite(PAUSE+offset, LOW);
    break;
    case 33: //REW
      mcp->digitalWrite(REW+offset, HIGH);
      delay(TRIG_MS);
      mcp->digitalWrite(REW+offset, LOW);
    break;
    case 41: //FWD
      mcp->digitalWrite(FWD+offset, HIGH);
      delay(TRIG_MS);
      mcp->digitalWrite(FWD+offset, LOW);
    break;
    case 27: //CAPSTAN SPEED CONTROL ENABLE/INHIBIT
      if(extra > 0) {
        mcp->digitalWrite(CAP+offset, HIGH);
      } else {
        mcp->digitalWrite(CAP+offset, LOW);
      }
    break;
    case 43: //CAPSTAN SPEED ADJUST
      analogWrite(capstan_cv_pin, extra);
    break;
    default:  //Invalid command code
      return;
  }

  Serial.print("SENT: ");
  Serial.print(machine_id);
  Serial.print(" command: ");
  Serial.print(cmdCode);  
  Serial.print(" extra: ");
  Serial.println(extra);  
}
