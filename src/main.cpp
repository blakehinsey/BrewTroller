#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

// Input and output points
  // pin_heaterRelay = digitalX?
  LiquidCrystal lcd(7,6,5,4,3,2); // !! need to lay out the pins
  const int pin_DispButton = 8;
  #define ONE_WIRE_BUS 9 // Data wire is plugged into digital pin 9 on the Arduino
  const int pin_TTarget = A0;
  int sens_TTarget;
  // push button 1 and 2

  OneWire oneWire(ONE_WIRE_BUS);   // Setup a oneWire instance to communicate with any OneWire devices
  DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature

// Timing and state variables
  unsigned long tCurrent;
  unsigned long tStateStart = 0; // tCurrent when BHeater changes
  unsigned long tState = 0; // time relative to tStateStart
  unsigned long tStateMin = 1; // minimum on/off time for the heater
  int hours;
  int minutes;
  bool BHeater = 0;
  bool BHeaterLast = 0;

// Display Setup
  int DispButtonState = 0;
  int DispPage = 0;
  int tStateHours;
  int tStateMins;


// Sensors and related thresholds
  float TAmb = 0;
  float TBrew = 0;
  float TTarget = 20; // initialise it at 20
  float Tdband = 0.5; // allowable temperature above or below target

void setup(){
  Serial.begin(9600);
  sensors.begin(); // start sensors library for dallas temp

  pinMode(pin_DispButton, INPUT); // button to change display page

  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
}


void loop(){
tCurrent = millis()/1000; // time in seconds

// Read sensors
  //sensTAmb = analogRead(A0);
  //TAmb = ((sensTAmb / 1024.0 * 5) - 0.5) * 100; // !! need to check this calibration - try the data logger at a few points
  sensors.requestTemperatures(); // get temperature from temp sensors
  TBrew = sensors.getTempCByIndex(0); // this gets the first (0) sensor on the bus
  TAmb = sensors.getTempCByIndex(1); // this gets the first (0) sensor on the bus

// Timers
tState = tCurrent - tStateStart;
  if (tState < 0) {
    tStateStart = tCurrent; //catch millis resetting
  }


// Heater logic
BHeaterLast = BHeater;

if (tState > tStateMin) { // only do anything with temperature data if time is greater than the state min
  if (BHeater == 0) {
    if (TBrew < (TTarget - Tdband)) {
      BHeater = 1;
      tStateStart = tCurrent;
    }
  }
  else {
    if (TBrew > (TTarget + Tdband)) {
      BHeater = 0;
      tStateStart = tCurrent;
    }
  }
}

// TTarget Setup - reads from the potentiometer and is mapped below
sens_TTarget = analogRead(pin_TTarget);
TTarget = map(sens_TTarget,0,1023,10,30); // input, input from, input to, output from, output to

// Heater control - what the f is going on here?
if (BHeater != BHeaterLast) { //!! is this correct for does not equal?
  //digitalWrite("""define the pin""", BHeater)
}
// Write to display
  DispButtonState = digitalRead(pin_DispButton); // check if the button is pressed

  if (DispButtonState == HIGH) {
    if (DispPage == 0) {
      DispPage = 1;
    }
    else {
      DispPage = 0;
    }
  }

  hours = tState / 3600;
  minutes = (tState % 3600) / 60;
if (DispPage == 0) {
  //write code to change display page
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ta:");
  lcd.print(TAmb,1);
  // print heater state
    if (BHeater == 1){
      lcd.print("[ON]");
    }
    else {
      lcd.print("[OF]");
    }

  // Format time
  if (hours < 10){lcd.print(0);}
  lcd.print(hours);
  lcd.print(":");
  if (minutes < 10){lcd.print(0);}
  lcd.print(minutes);

  lcd.setCursor(0,1); // second row
  lcd.print("TB:");
  lcd.print(TBrew,1);
  lcd.print("/Tgt:");
  lcd.print(TTarget,1);
}
else{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("This is page 2");
}





// Write to Display
/*
Page 1
     TA:16.5[ONF]1:20     (16char) Ambient temp / status / H:MM since change
     TB:19.7/Tgt:20.0     (16char) Brew temp / target temp
Page 2
     Tmn:12.3Tmx:21.5     (16 char) minimum temp / maximum temp
     _on1:20_off2:30_     (16 char) max time on / max time off
"""

"""
if the button is pressed, display 'Page 1' or 'Page 2' for 1 second
then set the page number variable
then write the contents of the Page
*/

// Serial output layout
Serial.println(sens_TTarget);
Serial.print("Heater?");
Serial.print(BHeater);
delay(250); // only do something every x milliseconds
}
