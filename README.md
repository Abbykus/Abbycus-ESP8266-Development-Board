# QDEV-ESP8266
![IMG_20220227_211106](https://user-images.githubusercontent.com/99380815/155886137-8354b025-064f-463b-b2ff-134bc2cd164e.jpg)


## DESCRIPTION
The QDEV-ESP8266 board is a multipurpose Internet Of Things (IOT) module based on the AI-Thinker ESP-12F development module (Espressif ESP8266 SOC). 
The QDEV-ESP8266 is a powerful and cost effective development/product ready platform with a 'dongle' style form factor which incorporates a male USB-A connector allowing direct plugin to a PC/laptop/Raspberry Pi/etc.

## FEATURES
- Espressif Tensilica L106 32 bit low power CPU.
- 80 / 160 Mhz CPU clock speed.
- 80 KBytes of SRAM.
- 4 MBytes of flash memory.
- WiFi module supports IEEE802.11 b/g/n protocol w/embedded TCPIP stack.
- Intergated Inverted F PCB antenna.
- 1 channel 10-bit Analog to Digital converter (ADC).
- 9 GPIO's available with PWM and low power wakeup capability.
- UART supporting baud rates up to 4.608 Mbits/sec.
- Auto-programming circuit (eliminates the 'boot' button).
- On board user LED (IO0).

## SPECIFICATIONS
### MECHANICAL
![PCB_QDEV_ESP8266_DIM_V1 2](https://user-images.githubusercontent.com/99380815/154401655-a657988f-43b5-4292-a79f-05efef374700.png)

### ELECTRICAL
#### *POWER*
The board can accept either 5 volts or 3.3 volts. 5 volts is normally supplied via the USB-A connector but can also be input on the +5V breakout pin.
5 volt power is regulated down to 3.3 volts using a compact linear regulator. 3.3 volts can also be input on the 3.3V breakout pin.

**NOTE:** The board is not 5V tolerant. Board power on the 3.3V breakout pin or any I/O pin should not exceed 3.3V.

Typical operating current can range from microamps in deep sleep mode to approximately 160 milliamps when transmitting WiFi data. 

#### *BREAKOUT PIN DESCRIPTION*
Signal designators on the bottom of the PCB rotating clockwise:
- **RST** Chip reset - active low.
- **ADC** 10 bit analog to digital converter (ADC).
- **EN** Chip enable - active high.
- **IO16** GPIO 16 / Wake from deep sleep.
- **IO14** GPIO 14 / HSPI CLK.
- **IO12** GPIO 12 / HSPI_MISO.
- **IO13** GPIO13 / HSPI_MOSI / UART0_CTS.
- **3.3V** Output from internal regulator or Input to board if not powered from USB.
- **CS0** Not user available.
- **MISO** Not user available.
- **IO9** Not user available.
- **GND** Board ground connect.
- **+5V** Connected to USB 5V input.
- **IO10** Not user available.
- **MOSI** Not user available.
- **SCLK** Not user available.
- **GND** Board ground connect.
- **IO15** GPIO15 / MTDO / HSPICS / UART0_RTS. ** STRAPPING PIN **
- **IO2** GPIO2 / UART1_TXD. ** STRAPPING PIN **
- **IO0** GPIO0 / HSPI_MISO / I2S_DATA. ** STRAPPING PIN **
- **IO4** GPIO4.
- **IO5** GPIO5 / IR_R.
- **RXD** UART0_RXD / GPIO3.
- **TXD** UART0_TXD / GPIO1. ** STRAPPING PIN **

#### *NOT USER AVAILABLE PINS*
Several pins labeled "Not user available" are used internally by the ESP-12F module. These signals are provided on the breakout pins but should not be connected externally.

#### *STRAPPING PINS*
Three GPIO pins are used by the ESP8266 to control boot mode. The pin level during boot is as follows:
- **IO0** UART download mode - pulldown. Boot from flash - floating or pullup. 
- **IO2** UART download mode - pullup. Boot from flash - floating or pullup.
- **IO15** UART download mode - pulldown. Boot from flash - pulldown.

Additionally TXD0 should be held high during boot.
The QDEV ESP8266 manages the state of the strapping pins during boot but any user circuitry connected to these pins should not interfere with the boot states shown above. 

## USAGE
The QDEV-ESP8266 is programmed through the USB-A connector.

## APPLICATION EXAMPLES
### Wireless UART Bridge
The QDEV-ESP8266 can function as a UART bridge between two or more computers. 

## FIRMWARE DEVELOPMENT
Firmware can be developed on several platforms such as the Arduino IDE, PlatformIO on VSCode, or the Espressif ESP-IDF development environment. 
The code examples included here were written using the [PlatformIO IDE](https://platformio.org/install/ide?install=vscode).

While the Arduino IDE is relatively simple, the advanced features of PlatformIO make it much more compelling and useful to developers. 

The Espressif ESP-IDF offers more detailed control of certain features but lacks the rich library support of either the Arduino IDE or PlatformIO and is specific to Espressif devices.


