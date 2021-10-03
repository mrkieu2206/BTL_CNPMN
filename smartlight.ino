#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

//Auth Token in the Blynk App.
char auth[] = "hKQnz7Ow84LaxXO2fx8RzabQp687rbVa";

// Your WiFi credentials.
char ssid[] = "VietKieu"; // Name of Wifi
char pass[] = "1234567891"; //Password of Wifi

int ldr = A5; //LDR PIN
int pir = 2; //HC-SR501 PIN
int modeButton = 3; //ModeButton PIN
int ledButton = 4; //LedButton PIN
int led = 13; //LED PIN
int ledAutoMode = 12; //LED AutoMode PIN

int manualModeOn = 0;
int lastManualModeOn = 0;
boolean ledState = false;
int modeButtonState = 0;
int lastModeButtonState = 0;
int ledButtonState = 0;
int lastLedButtonState = 0;

WidgetLCD lcd(V10); //LCD in Blynk App
WidgetLED pir1(V2); //HC-SR501 in Blynk App

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200
ESP8266 wifi(&Serial);

void setup()
{
  // Debug console
  Serial.begin(9600);
  delay(10);
  // Set ESP8266 baud rate
  Serial.begin(ESP8266_BAUD);
  delay(10);
  Blynk.begin(auth, wifi, ssid, pass);
  pinMode(ledAutoMode, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(pir, INPUT);
  pinMode(ldr, INPUT);
  pinMode(modeButton, INPUT);
  pinMode(ledButton, INPUT);
}

//Processing of Arduino Uno R3
void autoMode() {
  int ldrValue = analogRead(ldr);
  int pirValue = digitalRead(pir);
  if (ldrValue < 500 && pirValue == HIGH ) {
    ledState = true;
  } else {
    ledState = false;
  }
}

void manualMode() {
}

void buttonControl() {
  //mode button control
  modeButtonState =  digitalRead(3);
  if (modeButtonState != lastModeButtonState) {
    if (modeButtonState == HIGH) {
      manualModeOn = !manualModeOn;
    }
  }
  lastModeButtonState = modeButtonState;

  //led button control
  ledButtonState = digitalRead(4);
  if (ledButtonState != lastLedButtonState) {
    if (ledButtonState == HIGH) {
      ledState = !ledState;
      manualModeOn = 1;
    }
  }
  lastLedButtonState = ledButtonState;
}

void ledStateUpdate() {
  if (ledState == true) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
  if (manualModeOn == 0) {
    digitalWrite(ledAutoMode, HIGH);
  } else {
    digitalWrite(ledAutoMode, LOW);
  }
}


//Events in Blynk App
BLYNK_WRITE(V1) {
  int pinData = param.asInt();
  if (pinData == 1) {
    ledState = true;
    manualModeOn = 1;
  } else {
    ledState = false;
    manualModeOn = 1;
  }
}

BLYNK_WRITE(V0) {
  int pinData = param.asInt();
  if (pinData == 1) {
    manualModeOn = 1;
  } else {
    manualModeOn = 0;
  }
}
void setStateBlynkApp() {
  lcd.clear();
  if (manualModeOn == 1) {
    lcd.print(0, 0, "Manual MODE");
    Blynk.virtualWrite(V0, HIGH);
  } else {
    lcd.print(0, 0, "Auto MODE");
    Blynk.virtualWrite(V0, LOW);
  }
  if (ledState == true) {
    lcd.print(0, 1, "Den bat");
    Blynk.virtualWrite(V1, HIGH);
  } else {
    lcd.print(0, 1, "Den tat");
    Blynk.virtualWrite(V1, LOW);
  }
  if (digitalRead(2) == 1) {
    pir1.on();
  } else {
    pir1.off();
  }
  Blynk.virtualWrite(V5, analogRead(A5));
}


void loop()
{
  Blynk.run();
  if (manualModeOn == true) {
    manualMode();
  } else {
    autoMode();
  }
  buttonControl();
  ledStateUpdate();
  setStateBlynkApp();
}
