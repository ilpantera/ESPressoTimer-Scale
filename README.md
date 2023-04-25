# ESPressoTimer-Scale

This project consists of a scale for a portafilter machine. The timer is triggered when the espresso shot is poured. The code uses an SSD1306 display, an ESP8266, a HX711, a load cell and a touch button.

## Installation

To use this code, you will need to install the following libraries:

- Adafruit_SSD1306
- Adafruit_GFX
- ESP8266WiFi
- Wire
- Timer
- Arduino
- Pushbutton (by Pololu)
- HX711_ADC (by Olav Kallhovd)

## Usage

After uploading the code to the ESP8266 module, connect the components. Place the scale on your drip tray and connect the Reed Sensor to your coffee machine. When pouring a shot, the timer will start and the scale will tare automatically. The timing and weight of the shot will be displayed on the OLED display.
