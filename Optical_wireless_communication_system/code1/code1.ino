#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
#define TX_PIN 3
#define RX_PIN 2
const char* TEST_MSG = "PARIS";
#define DOT_START_MS 200
#define DOT_MIN_MS   20
#define STEP_MS      20
const char* MORSE_LETTERS[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
  "..-", "...-", ".--", "-..-", "-.--", "--.."
};
const char* MORSE_DIGITS[] = {
  "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."
};
char morseToChar(const char* m) {
  for (int i = 0; i < 26; i++) if (strcmp(m, MORSE_LETTERS[i]) == 0) return 'A' + i;
  for (int i = 0; i < 10; i++) if (strcmp(m, MORSE_DIGITS[i]) == 0) return '0' + i;
  return '?';
}
const char* getMorse(char c) {
  c = toupper(c);
  if (c >= 'A' && c <= 'Z') return MORSE_LETTERS[c - 'A'];
  if (c >= '0' && c <= '9') return MORSE_DIGITS[c - '0'];
  return "";
}
int dotDuration = DOT_START_MS;
int dashDur()   { return dotDuration * 3; }
int symGap()    { return dotDuration; }
int letterGap() { return dotDuration * 3; }
int wordGap()   { return dotDuration * 7; }
int dotThresh() { return dotDuration * 2; }
int letterThr() { return dotDuration * 2; }
int wordThr()   { return dotDuration * 5; }
enum TxState { TX_IDLE, TX_PULSE_ON, TX_PULSE_OFF, TX_LGAP, TX_WGAP, TX_DONE };
TxState txState = TX_IDLE;
int txMsgIdx = 0;
const char* txMorseCode = "";
int txMorseIdx = 0;
unsigned long txStart = 0;
unsigned long txDur = 0;
bool txComplete = false;
unsigned long txBeginTime = 0;
int txSymCount = 0;
void txBegin() {
  txMsgIdx = 0; txMorseIdx = 0; txComplete = false;
  txState = TX_IDLE; txSymCount = 0; txBeginTime = millis();
}
void txUpdate() {
  if (txComplete) return;
  unsigned long now = millis();
  switch (txState) {
    case TX_IDLE:
      if (TEST_MSG[txMsgIdx] == '\0') {
        digitalWrite(TX_PIN, LOW); txComplete = true; txState = TX_DONE; return;
      }
      if (TEST_MSG[txMsgIdx] == ' ') {
        txMsgIdx++; txState = TX_WGAP; txStart = now; txDur = wordGap();
      } else {
        txMorseCode = getMorse(TEST_MSG[txMsgIdx]); txMorseIdx = 0;
        digitalWrite(TX_PIN, HIGH); txState = TX_PULSE_ON; txStart = now;
        txDur = (txMorseCode[0] == '.') ? dotDuration : dashDur();
        txSymCount++;
      }
      break;
    case TX_PULSE_ON:
      if (now - txStart >= txDur) {
        digitalWrite(TX_PIN, LOW); txState = TX_PULSE_OFF; txStart = now; txDur = symGap();
      }
      break;
    case TX_PULSE_OFF:
      if (now - txStart >= txDur) {
        txMorseIdx++;
        if (txMorseCode[txMorseIdx] == '\0') {
          txMsgIdx++; txState = TX_LGAP; txStart = now; txDur = letterGap();
        } else {
          digitalWrite(TX_PIN, HIGH); txState = TX_PULSE_ON; txStart = now;
          txDur = (txMorseCode[txMorseIdx] == '.') ? dotDuration : dashDur();
          txSymCount++;
        }
      }
      break;
    case TX_LGAP: if (now - txStart >= txDur) txState = TX_IDLE; break;
    case TX_WGAP: if (now - txStart >= txDur) txState = TX_IDLE; break;
    default: break;
  }
}
char rxBuf[16];
int rxLen = 0;
char rxMsg[64];
int rxMsgLen = 0;
bool rxLastSig = false;
bool rxInPulse = false;
unsigned long rxPulseStart = 0;
unsigned long rxSilence = 0;
void rxReset() {
  rxLen = 0; rxMsgLen = 0; rxInPulse = false; rxLastSig = false;
  memset(rxBuf, 0, 16); memset(rxMsg, 0, 64); rxSilence = millis();
}
void rxFlushLetter() {
  if (rxLen > 0) {
    rxBuf[rxLen] = '\0';
    char c = morseToChar(rxBuf);
    Serial.print("Decoded Morse: "); Serial.print(rxBuf); Serial.print("  ->  "); Serial.println(c);
    if (rxMsgLen < 63) { rxMsg[rxMsgLen++] = c; rxMsg[rxMsgLen] = '\0'; }
    rxLen = 0; memset(rxBuf, 0, 16);
  }
}
void rxUpdate() {
  bool sig = (digitalRead(RX_PIN) == HIGH);
  unsigned long now = millis();
  if (sig && !rxLastSig) { rxInPulse = true; rxPulseStart = now; }
  if (!sig && rxLastSig && rxInPulse) {
    rxInPulse = false;
    unsigned long pd = now - rxPulseStart;
    rxSilence = now;
    if (rxLen < 15) rxBuf[rxLen++] = (pd <= (unsigned long)dotThresh()) ? '.' : '-';
  }
  if (!rxInPulse && rxLen > 0 && now - rxSilence >= (unsigned long)letterThr()) rxFlushLetter();
  rxLastSig = sig;
}
float dotToWPM(int dot_ms) { return 1200.0 / dot_ms; }
float calcBPS(int symbols, unsigned long ms) { return (ms == 0) ? 0 : (symbols * 1000.0) / ms; }
enum TestState { TEST_RUNNING, TEST_FAILED, TEST_FINISHED };
TestState testState = TEST_RUNNING;
int testRound = 1;
float maxBPS = 0; float maxWPM = 0; int maxDot = 0;
void printLCD(int dot, bool passed) {
  lcd.clear(); lcd.setCursor(0, 0); lcd.print(dot); lcd.print("ms ");
  lcd.print((int)dotToWPM(dot)); lcd.print("WPM");
  lcd.setCursor(0, 1); passed ? lcd.print("PASS") : lcd.print("FAIL MAX");
}
void printRound(int dot, bool passed, float bps, unsigned long txMs) {
  Serial.println("================================");
  Serial.print("Round: "); Serial.println(testRound);
  Serial.print("Dot Duration: "); Serial.print(dot); Serial.println(" ms");
  Serial.print("WPM: "); Serial.println(dotToWPM(dot), 1);
  Serial.print("TX Message: "); Serial.println(TEST_MSG);
  Serial.print("RX Message: "); Serial.println(rxMsg);
  Serial.print("Transmission Time: "); Serial.print(txMs); Serial.println(" ms");
  Serial.print("Data Rate: "); Serial.print(bps, 2); Serial.println(" bits/sec");
  Serial.print("Match: "); passed ? Serial.println("YES") : Serial.println("NO");
  Serial.println("================================");
}
void printFinalResult() {
  Serial.println("\n################################\n    MAX DATA RATE REACHED\n################################");
  Serial.print("Best Dot Duration: "); Serial.print(maxDot); Serial.println(" ms");
  Serial.print("Maximum WPM: "); Serial.println(maxWPM, 1);
  Serial.print("Maximum Data Rate: "); Serial.print(maxBPS, 2); Serial.println(" bits/sec");
  Serial.println("################################");
  lcd.clear(); lcd.setCursor(0, 0); lcd.print("MAX RATE:");
  lcd.setCursor(0, 1); lcd.print(maxBPS, 1); lcd.print(" bps");
}
void setup() {
  pinMode(TX_PIN, OUTPUT); pinMode(RX_PIN, INPUT);
  digitalWrite(TX_PIN, LOW); Serial.begin(9600);
  lcd.begin(16, 2); lcd.print("OWC TEST"); delay(2000);
  dotDuration = DOT_START_MS; rxReset(); txBegin();
}
void loop() {
  if (testState == TEST_FAILED || testState == TEST_FINISHED) return;
  txUpdate(); rxUpdate();
  if (txComplete) {
    unsigned long flushWait = millis() + letterGap() + 50;
    while (millis() < flushWait) rxUpdate();
    rxFlushLetter();
    unsigned long txTime = millis() - txBeginTime;
    float bps = calcBPS(txSymCount, txTime);
    bool passed = (strcmp(TEST_MSG, rxMsg) == 0);
    printRound(dotDuration, passed, bps, txTime);
    printLCD(dotDuration, passed);
    if (passed) { maxBPS = bps; maxWPM = dotToWPM(dotDuration); maxDot = dotDuration; }
    if (!passed) { testState = TEST_FAILED; printFinalResult(); return; }
    if (dotDuration <= DOT_MIN_MS) { testState = TEST_FINISHED; printFinalResult(); return; }
    testRound++; dotDuration -= STEP_MS; delay(1000); rxReset(); txBegin();
  }
}