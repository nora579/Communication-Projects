#include <LiquidCrystal.h>
LiquidCrystal lcd(7,8,9,10,11,12);
 // Pins
#define TX 3
#define RX 2
#define DOT_BTN 4
#define DASH_BTN 5
// Timing Constants
#define DOT 1000
#define DASH 3000
#define GAP 1000
#define LETTER 3000
#define WORD 7000
#define RX_DOT 2000
// Morse Code Dictionaries
const char* letters[]={
".-","-...","-.-.","-..",".","..-.","--.","....","..",".---",
"-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-",
"..-","...-",".--","-..-","-.--","--.."
};

const char* digits[]={
"-----",".----","..---","...--","....-",
".....","-....","--...","---..","----."
};

char txBuf[16],txMsg[64],rxBuf[16],rxMsg[64];
int txLen=0,txMsgLen=0,rxLen=0,rxMsgLen=0;
bool lastDot=1,lastDash=1,lastSig=0,inPulse=0, wordDetected = 0;
unsigned long tRelease=0,pulseStart=0,rxSilence=0,blinkStart=0,blinkDur=0;

// TX State Machine States
enum State{IDLE,ON,OFF};
State blink=IDLE;
// Morse to letter
char morseToChar(const char* m){
  for(int i=0;i<26;i++) if(strcmp(m,letters[i])==0) return 'A'+i;
  for(int i=0;i<10;i++) if(strcmp(m,digits[i])==0) return '0'+i;
  return '?';
}
// Handles the completion of a Morse sequence in transmission
void txFlush(){
  if(!txLen) return;
  txBuf[txLen]='\0';

  lcd.setCursor(4,0);
  lcd.print("            ");
  lcd.setCursor(4,0);
  lcd.print(txBuf);

  txLen=0;
  memset(txBuf,0,16);
  tRelease=millis();
  }
// Transmission logic: monitors buttons and controls the IR LED
void handleTX(){
  bool dot=digitalRead(DOT_BTN);
  bool dash=digitalRead(DASH_BTN);
  unsigned long now=millis();

  switch(blink){
    case IDLE:
      if(dot==LOW && lastDot){    // If Dot button is pressed
        txBuf[txLen++]='.';
        lcd.setCursor(4,0);
        lcd.print("            ");
        lcd.setCursor(4,0);
        lcd.print(txBuf);

        Serial.print(".");
        digitalWrite(TX,HIGH);

        blink=ON;
        blinkStart=now;
        blinkDur=DOT;
      }
      else if(dash==LOW && lastDash){   // If Dash button is pressed
        txBuf[txLen++]='-';
        lcd.setCursor(4,0);
        lcd.print("            ");
        lcd.setCursor(4,0);
        lcd.print(txBuf);

        Serial.print("-");
        digitalWrite(TX,HIGH);

        blink=ON;
        blinkStart=now;
        blinkDur=DASH;
      }
      break;

    case ON:   // Keep LED ON for the specified duration
      if(now-blinkStart>=blinkDur){
        digitalWrite(TX,LOW);
        blink=OFF;
        blinkStart=now;
        blinkDur=GAP;
      }
      break;

    case OFF:  // Ensure a GAP between pulses
      if(now-blinkStart>=blinkDur){
        blink=IDLE;
        tRelease=now;
      }
      break;
      }
  if(blink==IDLE && txLen){
      unsigned long silence=now-tRelease;
      if(silence>=WORD){
        txFlush();
        txMsg[txMsgLen++]=' ';
        txMsg[txMsgLen]='\0';
      }
      else if(silence>=LETTER) txFlush();
      }

  lastDot=dot;
  lastDash=dash;
}
// Translates received Morse into text and displays it
void rxFlush(){
  if(!rxLen) return;
  rxBuf[rxLen]='\0';
  char c=morseToChar(rxBuf);

  Serial.println("RX: ");
  Serial.print(rxBuf);
  Serial.print(" -> ");
  Serial.println(c);

  if(rxMsgLen<63){
    rxMsg[rxMsgLen++]=c;
    rxMsg[rxMsgLen]='\0';
  }
  lcd.setCursor(4,1);
  lcd.print("                ");
  lcd.setCursor(4,1);
  lcd.print(rxMsg);
  rxLen=0;
  memset(rxBuf,0,16);
}
// Receiving logic: monitors the IR sensor and measures pulse durations
void handleRX(){
  bool sig=digitalRead(RX);
  unsigned long now=millis();
  if(sig && !lastSig){
    inPulse=1;
    pulseStart=now;
  }
  if(!sig && lastSig){
    inPulse=0;
    unsigned long p=now-pulseStart;
    rxSilence=now;
    rxBuf[rxLen++]=(p<=RX_DOT)?'.':'-';
  }

  if(!inPulse && rxLen && now-rxSilence>=LETTER)
  rxFlush();

 if (!inPulse && !rxLen && rxMsgLen && now - rxSilence >= WORD) {
    if (!wordDetected) {
      Serial.println("7 Seconds Silence: New Word Detected");
      if (rxMsg[rxMsgLen - 1] != ' ') {
        rxMsg[rxMsgLen++] = ' ';
        rxMsg[rxMsgLen] = '\0';
      }
      wordDetected = 1;
    }
  }
  lastSig = sig;
}
// Initialization
void setup(){
  pinMode(TX,OUTPUT);
  pinMode(RX,INPUT);
  pinMode(DOT_BTN,INPUT_PULLUP);
  pinMode(DASH_BTN,INPUT_PULLUP);

  digitalWrite(TX,LOW);
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.print("OWC Button Mode");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TX:");
  lcd.setCursor(0,1);
  lcd.print("RX:");
  tRelease=millis();
  rxSilence=millis();
  Serial.println(" BUTTON MODE ");
}
// Main loop
void loop(){
  handleTX();
  handleRX();
}