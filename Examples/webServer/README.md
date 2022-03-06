## WEB SERVER SIMPLE EXAMPLE

The example code was originally published by Majenko Technologies and demonstrates a relatively simple web page that redraws a random graph every 5 seconds.

### USAGE
- Create an empty project in PlatformIO.
- Replace the default ***'main.cpp'*** file in the *'src'* directory with the ***'main.cpp'*** file from the example.
- Replace the default ***'platformio.ini'*** file in the project directory with the ***'platformio.ini'*** file from the example folder.
- Near the top of the ***'main.cpp'*** file enter the SSID and PASSWORD of your router.
- Compile and download the code to a QDEV-ESP8266 device.
- Monitor the serial port and when the device connects to your router you should see the *local IP* of the node; something like *192.168.1.15.*
- On your PC, laptop, or smartphone browser enter the *local IP* of the node in the browser address bar.
- You should see a **HELLO from ESP8266** message and a random graph displayed.
- Enjoy!
