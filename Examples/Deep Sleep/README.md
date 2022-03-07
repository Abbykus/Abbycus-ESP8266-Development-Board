## ESP8266 SLEEP 
The ESP8266 can enter a deep sleep mode which significantly reduces the ESP8266 current consumption to around 20 microamps (MCU only).
In deep sleep all MCU peripherals are powered down except the Real Time Clock (RTC). 

The ESP8266 can wake from deep sleep by connecting GPIO16 to the Reset (RST) breakout pin. See the sketch 'main.cpp' for a simple example. The ESP8266 will enter deep sleep and at the end of the timeout period it will assert GPIO16 which causes the MCU to be reset.

For more information see:

- [ESP8266 Deep Sleep](https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/).
- [ESP8266 (ESP-12F) deep sleep and light sleep](https://kevinstadler.github.io/notes/esp8266-deep-sleep-light-sleep-arduino/).
