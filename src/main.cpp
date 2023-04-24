#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Timer.h>
#include <Arduino.h>
#include <Pushbutton.h>
#include <HX711_ADC.h>

#include <Fonts/FreeSans12pt7b.h>

//Definition of PINs
const int PUMP_PIN = 2; // Reed Sensor / Pump
const int HX711_dout = 12; //mcu > HX711 dout pin
const int HX711_sck = 13; //mcu > HX711 sck pin
const int BUTTON_PIN = 14; // Touch Button
const float calibrationValue = 2626.43;

//Define Display, Loadcell, Tare-Button & Timer
Adafruit_SSD1306 display(128, 64, &Wire, -1);
HX711_ADC LoadCell(HX711_dout, HX711_sck);
Pushbutton button(BUTTON_PIN);
Timer PumpTimer;

//Create Variables
float weightreading = 0;
bool Tared = false;

void PumpStarted ();
void ButtonPressed();
void displayCenterText(String line1, String line2, int16_t lineSpacing, int16_t fontSize);
void checkUpdateDisplay();


void setup() {
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);delay (5);
  Serial.println("Starting...");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  //Set Font
  //display.setFont(&FreeSans12pt7b);
  display.display();
  pinMode(PUMP_PIN, INPUT);
  Serial.println("Pump initialized");
  LoadCell.begin();
  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Scale initialized");
  }
  Serial.println("Startup is complete");
}

void loop() {
  ButtonPressed();
  PumpStarted();
  checkUpdateDisplay();
}

void PumpStarted(){
  if (digitalRead(PUMP_PIN)==LOW && PumpTimer.state()==STOPPED) {
    PumpTimer.start();
    LoadCell.tareNoDelay();
    Serial.println("Pump started & scale tare");
  }
  else if (digitalRead(PUMP_PIN)==HIGH && PumpTimer.state()==RUNNING) {
    PumpTimer.stop();
    Serial.println("Pump stopped");
  }
}

void ButtonPressed() {
  if (button.getSingleDebouncedPress()){
    LoadCell.tareNoDelay();
    displayCenterText("Tara", "", 0,0);
    Serial.println("Tare Button pressed, tare done");
    Tared = true;
    delay(500);
  }
}

void displayCenterText(String line1, String line2, int16_t lineSpacing, int16_t fontSize) {
  int16_t x1, y1, x2, y2;
  uint16_t w1, h1, w2, h2;
  
  //Set font site
  //display.setTextSize(fontSize);

  // Get text bounds for line 1
  display.getTextBounds(line1, 0, 0, &x1, &y1, &w1, &h1);

  // Get text bounds for line 2 (if present)
  if (line2 != "") {
    display.getTextBounds(line2, 0, 0, &x2, &y2, &w2, &h2);
  } else {
    h2 = 0;
    }

  // Calculate the position to center the text
  int16_t xPos1 = (display.width() - w1) / 2;
  int16_t yPos1 = (display.height() - h1 - h2 - lineSpacing) / 2;

  int16_t xPos2 = (display.width() - w2) / 2;
  int16_t yPos2 = yPos1 + h1 + lineSpacing;

  // Clear the display and set the cursor position
  display.clearDisplay();

  if (line2 == "") {
    // Print one line of text
    display.setCursor(xPos1, yPos1);
    display.println(line1);
  } else {
    // Print two lines of text
    display.setCursor(xPos1, yPos1);
    display.println(line1);

    display.setCursor(xPos2, yPos2);
    display.println(line2);
  }

  // Update the display
  display.display();
}

void checkUpdateDisplay() {
  // Initialized variables
  static float prevWeight = 1909;
  static int16_t prevSeconds = 1909;

  bool weightChanged = false;
  bool timerChanged = false;

  int16_t timerSeconds = PumpTimer.read()/1000;
  
  LoadCell.update();
  weightreading = LoadCell.getData();
  
  if (weightreading != prevWeight) {
    prevWeight = weightreading;
    weightChanged = true;
  }
  else if (timerSeconds != prevSeconds) {
    prevSeconds = timerSeconds;
    timerChanged = true;
  }

  if (weightChanged || timerChanged || Tared) {
      weightreading = LoadCell.getData();
      // Print time
      String timerStr = String(timerSeconds)+ "s";
      //Print Weight
      String weightStr;
        if (weightreading <0.1 && weightreading>-0.1){
          weightStr = "0.0g";
        }
        else{
            weightStr = String(weightreading, 1) + "g";
        }    
    Tared = false;
    Serial.println("Status: " + timerStr + " | " + weightStr);
    displayCenterText(timerStr, weightStr, 5, 0);
  }
}