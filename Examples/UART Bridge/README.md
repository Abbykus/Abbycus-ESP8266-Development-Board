## UART WIRELESS BRIDGE

This project example demonstrates how to create a peer-to-peer wireless serial UART bridge between any combination of PC's, laptops, Raspberry Pi's, etc.
The bridge operates as if there was a direct serial cable between the two serial devices and uses the Espressif ESP-NOW ad-hoc network to impliment the bridge without the need for an intermediate router.
Pairing between the devices is automatic.

Serial data is sent in sequential packets using a 'filter' key to validate messages on this network. The default key is 'DEADBEEF' but can be any 32 bit value.

Missing packets are detected if they are received out of sequence and retries are used to recover lost packets.
The code is primarily designed for 2 nodes but could be expanded to service multiple nodes.

### USAGE
- Create a new empty PlatformIO project.
- Replace the *'main.cpp'* file in the *'src'* directory with the *'main.cpp'* file from the example folder.
- Replace the default *'platformio.ini'* file with the *'platformio.ini'* file from the example folder.
- Compile and download the code to two QDEV-ESP8266 devices and install each one in a different 'PC'.
- Open a serial port emulator in Windows, MACOS, Raspbian, or Linux.
- Now you should be able to type on one machine and receive on the other to demonstrate bi-directional communications.
- NOTE: The wireless UART emulation is half-duplex only since the devices only have one radio which cannot transmit and receive simultaneously.
- Enjoy!
