#include <LiquidCrystal.h>
LiquidCrystal lcd(7,8,9,10,11,12);

#define TX_PIN 3  // IR LED output pin
#define RX_PIN 2  // photodiode / IRLINK input pin

const char* letters[]={".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--.."};
const char* digits[]={"-----",".----","..---","...--","....-",".....","-....","--...","---..","----."};
// Transmission times
#define DOT_TIME 200
#define DASH_TIME 600
#define SYMBOL_GAP 200
#define LETTER_GAP 600
#define WORD_GAP 1400
// Receiver times
#define DOT_LIMIT 400
#define LETTER_LIMIT 500
#define WORD_LIMIT 1000
#define MSG_TIMEOUT 3500
// TX state machine states
enum TxState{IDLE,PULSE_ON,PULSE_OFF,GAP_LETTER,GAP_WORD,FINISHED};

const char* message="OWC LINK";  // Message
const char* currentMorse="";

char morseBuffer[16],decodedText[17];

int msgIndex=0,morseIndex=0,morseLen=0,textLen=0;

bool txDone=false,lastSignal=false,pulseActive=false,receiving=false,messageShown=false; // State flags

unsigned long stateStart=0,stateTime=0,pulseStart=0,silenceStart=0; // Timing variables

TxState txState=IDLE;
 // letter to morse
const char* getMorse(char c){
  c=toupper(c);
  if(c>='A'&&c<='Z') return letters[c-'A'];
  if(c>='0'&&c<='9') return digits[c-'0'];
  return "";
}
 // morse to letter
char morseToChar(const char* code){
  for(int i=0;i<26;i++) if(strcmp(code,letters[i])==0) return 'A'+i;
  for(int i=0;i<10;i++) if(strcmp(code,digits[i])==0) return '0'+i;
  return '?';
}
// Reset TX state and display the message being sent
void startTransmission(){
  msgIndex=0;
  morseIndex=0;
  txDone=false;
  txState=IDLE;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sending:");
  lcd.setCursor(8,0);
  lcd.print(message);
  lcd.setCursor(0,1);
  lcd.print("Recvd:");
  Serial.print("Sending: ");
  Serial.println(message);
}
// TX state machine: drives IR LED to blink Morse pulses
void updateTX(){
  if(txDone) return;
  unsigned long now=millis();

  switch(txState){

    case IDLE:  // load next character from message
      if(message[msgIndex]=='\0'){
        digitalWrite(TX_PIN,LOW);
        txDone=true;
        txState=FINISHED;
        return;
      }

      if(message[msgIndex]==' '){
        digitalWrite(TX_PIN,LOW);
        msgIndex++;
        txState=GAP_WORD;
        stateStart=now;
        stateTime=WORD_GAP;
      }

      else{
        currentMorse=getMorse(message[msgIndex]);
        morseIndex=0;
        digitalWrite(TX_PIN,HIGH);
        txState=PULSE_ON;
        stateStart=now;
        stateTime=(currentMorse[morseIndex]=='.')?DOT_TIME:DASH_TIME;
      }
      break;

    case PULSE_ON: // LED is ON wait for dot or dash duration then turn OFF
      if(now-stateStart>=stateTime){
        digitalWrite(TX_PIN,LOW);
        txState=PULSE_OFF;
        stateStart=now;
        stateTime=SYMBOL_GAP;
      }
      break;

    case PULSE_OFF: // LED is OFF wait symbol gap then go to next dot or dash
      if(now-stateStart>=stateTime){
        morseIndex++;

        if(currentMorse[morseIndex]=='\0'){
          msgIndex++;
          txState=GAP_LETTER;
          stateStart=now;
          stateTime=LETTER_GAP;
        }

        else{
          digitalWrite(TX_PIN,HIGH);
          txState=PULSE_ON;
          stateStart=now;
          stateTime=(currentMorse[morseIndex]=='.')?DOT_TIME:DASH_TIME;
        }
      }
      break;

    case GAP_LETTER: // pause between letters
      if(now-stateStart>=stateTime) txState=IDLE;
      break;

    case GAP_WORD: // pause between words
      if(now-stateStart>=stateTime) txState=IDLE;
      break;

    case FINISHED:
      break;
  }
}
// Convert accumulated morseBuffer into a character
void decodeLetter(){
  if(morseLen==0) return;

  morseBuffer[morseLen]='\0';
  char c=morseToChar(morseBuffer);
    // Serial output
  Serial.print(morseBuffer);
  Serial.print(" -> ");
  Serial.println(c);

  if(textLen<16){
    decodedText[textLen++]=c;
    decodedText[textLen]='\0';

    lcd.setCursor(7,1);
    lcd.print("         ");
    lcd.setCursor(7,1);
    lcd.print(decodedText);
  }

  morseLen=0;
  memset(morseBuffer,0,sizeof(morseBuffer));
}

// Clear all receiver buffers and flags to be ready for next message
void resetReceiver(){
  morseLen=0;
  textLen=0;
  receiving=false;
  messageShown=false;
  memset(morseBuffer,0,sizeof(morseBuffer));
  memset(decodedText,0,sizeof(decodedText));
  silenceStart=millis();
}
// RX: reads Pin 2 and builds morse buffer
void updateRX(){
  bool signal=digitalRead(RX_PIN);
  unsigned long now=millis();

  if(signal&&!lastSignal){
    pulseActive=true;
    pulseStart=now;
    messageShown=false;
    receiving=true;

    unsigned long silence=now-silenceStart;

    if(silence>=WORD_LIMIT&&silence<MSG_TIMEOUT&&morseLen==0&&textLen>0){
      if(textLen<16){
        decodedText[textLen++]=' ';
        decodedText[textLen]='\0';
      }
    }
  }

  if(!signal&&lastSignal){
    pulseActive=false;
    unsigned long duration=now-pulseStart;
    silenceStart=now;

    if(morseLen<15)
      morseBuffer[morseLen++]=(duration<=DOT_LIMIT)?'.':'-';
  }

  if(!pulseActive&&morseLen>0)
    if(now-silenceStart>=LETTER_LIMIT)
      decodeLetter();

  if(receiving&&!pulseActive&&morseLen==0&&textLen>0&&!messageShown){
    if(now-silenceStart>=MSG_TIMEOUT){
      messageShown=true;
      resetReceiver();
    }
  }

  lastSignal=signal;
}
// Initialization
void setup(){
  pinMode(TX_PIN,OUTPUT);
  pinMode(RX_PIN,INPUT);
  digitalWrite(TX_PIN,LOW);
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("OWC Ready");

  Serial.println("=== OWC System ===");
  Serial.println("===========================");
  delay(1500);

  silenceStart=millis();
  startTransmission();
}
// Main loop
void loop(){
  updateTX();
  updateRX();

  if(txDone){
    delay(LETTER_LIMIT+100);
    updateRX();
    delay(3000);
    resetReceiver();
    startTransmission();
  }
}