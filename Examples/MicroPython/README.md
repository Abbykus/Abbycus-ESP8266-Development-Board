## MicroPython ON THE ESP8266

The QDEV-ESP8266 is capable of running the MicroPython interpreted language. 
MicroPython is a memory efficient and scaled down version of the popular Python high level scripting language.

Please read [MicroPython language and implementation](https://docs.micropython.org/en/latest/reference/index.html) for more information.

### INSTALL MICROPYTHON 

To install MicroPython firmware on the QDEV ESP8266 board see [Quick reference for the ESP8266](https://docs.micropython.org/en/latest/esp8266/tutorial/intro.html#intro).

Once you have installed MicroPython on the QDEV ESP8266 board you can connect to the board via the Serial monitor in your development environment. You should expect to see a '>>>' prompt where you can begin to enter MicroPython commands or entire scripts manually.

### INSTALL AMPY
In order to develop scripts that autorun after the QDEV ESP8266 module boots, you will need the AdaFruit MicroPython Tool ***ampy*** which is a utility to interact with a MicroPython board (QDEV ESP8266) over its serial connection.

***Ampy*** is a simple command line tool to manipulate files and run code on a MicroPython board over its serial connection. With ***ampy*** you can send files from your computer to the board's file system, download files from a board to your computer, and even send a Python script to a board to be executed.

Visit [Here](https://pypi.org/project/adafruit-ampy/) for instructions on installation and use of ***ampy***. 

