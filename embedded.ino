#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SevSeg.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int L1 = 2;
int L2 = 3;
int L3 = 4;
int Thermistor = A8;
int Buzzer = 8;
int StartButton = A13;
int AddButton = A14;
int SubtractButton = A15;
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
SevSeg sevseg;

unsigned long ledPreviousMillis = 0;  // millis for blinking LED
unsigned long ledPreviousMillis2 = 0; // millis for timer function LED
unsigned long ledIndicatorMillis = 0; // millis for indicator LED
unsigned long lcdMillis = 0; // millis for LCD
int indicatorState = LOW;
const long interval = 1000;
int ledState = LOW; 
int led2State = LOW;
int alarmState = LOW; 

int deciSeconds = 600;
bool running = false;

void setup(){
  Serial.begin(2000000);
  pinMode(Thermistor, INPUT);
  pinMode(Buzzer, INPUT);
  pinMode(StartButton, INPUT);
  pinMode(AddButton, INPUT);
  pinMode(SubtractButton, INPUT);

  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  
  byte numDigits = 4;
  byte digitPins[] = {33, 32, 31, 30};
  byte segmentPins[] = {22, 23, 24, 25, 26, 27, 28, 29};
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = true; // Default 'false' is Recommended
  bool leadingZeros = true; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,1);
  lcd.print("stefastra");
}

void loop(){
  unsigned long currentMillis = millis();
  if (currentMillis - ledPreviousMillis >= interval) {
    ledPreviousMillis = currentMillis;

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(L3, ledState);
  }

  
  Vo = analogRead(Thermistor);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;

    // subtract button
  if (digitalRead(A13) == LOW) {
    tone(Buzzer, 440, 100); // play tone 57 (A4 = 440 Hz)  
    indicatorState = HIGH;
    ledIndicatorMillis = millis();
    if(!running && deciSeconds - 100 > 0){
      deciSeconds = deciSeconds - 100;
    }
    delay(100);
  }
  
    
  // start/stop button
  if (digitalRead(A14) == LOW) {
    tone(Buzzer, 494, 100); // play tone 59 (B4 = 494 Hz)
    indicatorState = HIGH;
    ledIndicatorMillis = millis();
    if(!running){
      running = true;
    }else{
      running = false;
    }
  }
  
  // add button
  if (digitalRead(A15) == LOW) {
    tone(Buzzer, 523, 100); // play tone 60 (C5 = 523 Hz)
    indicatorState = HIGH;
    ledIndicatorMillis = millis();
    if(!running && deciSeconds + 100 <= 6000){ //6000 deciseconds is 10 minutes
      deciSeconds = deciSeconds + 100;
    }
    delay(100);
  }

  if(millis() - ledIndicatorMillis > 50){
    indicatorState = LOW;
  }
    digitalWrite(L1, indicatorState);

  static unsigned long timer = millis(); 
  if (millis() - timer >= 100) {
    timer += 100;
    if(running){
      deciSeconds--; // 100 milliSeconds is equal to 1 deciSecond
    if (deciSeconds / 100 == 0){
    if (led2State == LOW) {
      led2State = HIGH;
        } else {
      led2State = LOW;
        }
        digitalWrite(L2,led2State);
      }
    }
    digitalWrite(L3, ledState);
    if (deciSeconds <= 0) { // time is up
      deciSeconds=0;
      tone(Buzzer, 523, 100);
      delay(300);
      running = false;
      
      unsigned long currentMillis = millis();
        if (currentMillis - ledPreviousMillis2 >= interval) {
      ledPreviousMillis = currentMillis;

        if (alarmState == LOW) {
          alarmState = HIGH;
      } else {
          alarmState = LOW;
      }

    digitalWrite(L2, led2State);
    digitalWrite(L3, ledState);
  }
    }
    sevseg.setNumber(deciSeconds, 1);
  }
  if(millis() - lcdMillis > 1000){
    lcd.clear();
    lcd.print("Temp: "); 
    lcd.print(T);
    lcd.print(" C");
    lcd.setCursor(0,1);
    lcd.print("stefastra");
    lcdMillis = millis();
    }

  sevseg.refreshDisplay();
}
