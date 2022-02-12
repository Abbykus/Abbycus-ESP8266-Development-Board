/********************************************************************
 * @file main.h
 * @author J. Hoeppner / Abbykus
 * @brief Header file for the ESP8266 UART bridge project
 * @version 0.1
 * @date 2022-01-25
 * 
 * @copyright Copyright (c) 2022
 * 
 * @INTRODUCTION:
 * The code demonstrates a method of creating a wireless UART bridge without the need to communicate
 * through a router. This example uses the ESP-NOW peer-peer protocol to establish communication 
 * between devices using their MAC addresses as their unique ID. 
 * This code is designed and tested on the Abbykus ESP8266-TINY-DEV modules.
 * 
 * @THEORY:
 * At startup each device will broadcast a PAIRING REQ to open communications with another device.
 * During pairing the devices exchange their hardware assigned Mac addresses which are their network ID's.
 * When both devices have completed the pairing REQ/ACK handshake, communication is established.
 * Only 'compatible' devices can communicate. A 'compatible' device is determined by the 32 bit
 * MESG_FILTER_KEY value. The value can be any number but it has to be the same for all devices.
 * 
 * @EXAMPLE:
 * Install this code into a pair of ESP8266-TINY-DEV modules and insert into USB slots of different systems. 
 * Open a serial terminal on both systems with the proper serial COM PORT and the BAUDRATE set to
 * the same value as the SERIAL_BAUDRATE shown below. 
 * The user can then send and receive serial data between the systems.
 * 
 * @NOTE:
 * The communication is half duplex because there is only one radio which can't send & receive at the
 * same time. Therefore it is possible that data may be lost if both ends are trying to send at the same
 * time.
 * The code example does detect lost packets and will react with a resend message to try and recover.
 * 
 * @OPTIONS:
 * SERIAL_BAUDRATE - this should be the same baudrate as set in the system application.
 * SERIAL_DATA_IS_BINARY - if this is enabled, serial data is sent/received as binary data, otherwise data
 * are treated as ASCII characters. 
 * MESG_FILTER_KEY - Message filter value.
 * 
 * 
 */
#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

/* USER SETTINGS */
#define SERIAL_BAUDRATE       115200
#define SERIAL_DATA_IS_BINARY       // comment out to send/rcv ascii data

/* data packet defines */
#define ESP_NOW_PACKET_SZ     250
#define PACKET_HDR_SIZE       8
#define MAX_PAYLOAD_SIZE      240
#define MESG_FILTER_KEY       0xDEADBEEF
#define ESP_NOW_CHANNEL       0

/* board defines */
#define ONBOARD_LED           0        // led is on gpio 0

/*
 * USB-NOW packet structure
 */
#pragma pack(push, 1)               // packed structure (no alignment padding)
typedef struct {
   uint8_t packet_type;             // pairing, data, ack, etc. See packet type enums
   uint32_t mesg_filter;            // bitwise message filtering. 0 = accept all.
   uint16_t packet_seq;              // packet num increments for each packet
   uint8_t payload_len;             // length of payload data (max 240 bytes)   
   char payload[MAX_PAYLOAD_SIZE];  // data sent
   uint16_t spare;                  // for future...
} MESG_PACKET;
#pragma pack(pop)

/* Packet types */
enum {
   TYPE_NONE,
   PAIRING_REQ,
   PAIRING_ACK,
   DATA_ALL,
   DATA_PARTIAL,
   DATA_ACK,
   DATA_RESEND_LAST,
   DATA_RESEND_ALL,
   DATA_RESEND_PARTIAL,   
   KEEP_ALIVE_REQ,
   KEEP_ALIVE_ACK,
}; 

/* send data status codes */
#define   SEND_OK                0
#define   SEND_FAIL              1
#define   SEND_WAITING_STATUS    99

/* Create a FIFO ring buffer for queing multiple received packets */
#define RCV_PACKET_BUFR_SZ    6
typedef struct {
   uint8_t cur_ring_index;
   uint8_t next_ring_index;
   uint8_t ring_count;
   MESG_PACKET rcvMesgPacket[RCV_PACKET_BUFR_SZ];
}RING_BUFR;
