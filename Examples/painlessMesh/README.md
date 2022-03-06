## PAINLESS MESH NETWORK EXAMPLE

This example code demonstrates a basic implementation of ad-hoc mesh networking. 

Mesh WiFi works by using two or more devices or “nodes” to create a secure and strong wifi network. These nodes are important in efficiently delivering data over WiFi, as each node serves as a “daisy chain” for other nodes in the system. Even nodes that are outside the reach of some nodes can still participate in the network.

*painlessMesh* is a library that takes care of the particulars of creating a simple mesh network using esp8266 and esp32 hardware. The goal is to allow the programmer to work with a mesh network without having to worry about how the network is structured or managed.

Please visit the [painlessMesh](https://gitlab.com/painlessMesh/painlessMesh) site for more information and additional examples.

Also note that the library dependencies seen in the *platformio.ini* file may not be the latest release versions. 

### USAGE
The example code was compiled and downloaded to the target device using the [PlatformIO](https://platformio.org/) development environment running on top of Microsoft's Visual Studio Code (VSCode ).
- Create an empty Platformio project.
- Replace the empty main.cpp in the 'src' directory with the main.cpp in the eaxample.
- Add the library dependencies as directed in the [painlessMesh](https://gitlab.com/painlessMesh/painlessMesh) page.
- Compile and download to two or more QDEV-ESP8266 devices (nodes).

You should expect to see that all nodes in the network are exchanging messages.
