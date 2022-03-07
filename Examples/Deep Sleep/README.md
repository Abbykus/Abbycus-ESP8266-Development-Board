## ESP8266 DEEP SLEEP
The ESP8266 can enter a deep sleep mode which reduces the ESP8266 current consumption to around 20 microamps.
In deep sleep all MCU peripherals are powered down except the Real Time Clock (RTC). 

The ESP8266 can wake from deep sleep by connecting GPIO16 to the Reset (RST) breakout pin. See the sketch 'main.cpp' for a simple example.

See also:

[ESP8266 Deep Sleep](https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/).

[ESP8266 (ESP-12F) deep sleep and light sleep](https://kevinstadler.github.io/notes/esp8266-deep-sleep-light-sleep-arduino/).
