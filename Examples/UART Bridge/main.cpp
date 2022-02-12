#include "main.h"

// mac addrs containers
static uint8_t ourMac[8]; 
static uint8_t broadcastMac[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0};
static uint8_t peerMac[8];

RING_BUFR ringBuf;

uint16_t last_rcvd_packet_seq;          // copy of last packet # rcvd 
uint16_t next_xmit_packet_seq;         // incrementing packet # to send

static MESG_PACKET xmtMesgPacket;
static MESG_PACKET xmtMesgPacket_last;

bool peer_paired = false;
volatile uint8_t send_packet_status;

uint32_t pairing_interval;
uint32_t pairing_timer;

volatile bool peer_connected = false;
uint8_t resend_counter;

/**
 * @brief Print MAC address. Used during debug only.
 * 
 * @param mac - 6 byte mac addr
 */
void printMacAddr(uint8_t * mac)
{
   uint8_t i;
   Serial.print("Mac=");
   for(i=0; i<6; i++)
   {
      Serial.print(" 0x");
      Serial.print(mac[i], HEX);
   }
   Serial.println("");
}


/********************************************************************
 * @brief ESP-NOW receive data callback
 * 
 * @param macadr - 6 byte mac adrs of the sender (peer)
 * @param incomingData - data packet (max 250 bytes)
 * @param len - length of the sender data including the 4 byte packet header
 */
void onDataRcv(uint8_t *macadr, uint8_t *incomingData, int len) 
{
   uint32_t *p;
   uint32_t filter;
   
   p = reinterpret_cast <uint32_t *>(incomingData +1);
   filter = *p;
   if(filter == MESG_FILTER_KEY)
   {
      memcpy(&ringBuf.rcvMesgPacket[ringBuf.next_ring_index], incomingData, len);  // save data in packet array
      if(++ringBuf.next_ring_index >= RCV_PACKET_BUFR_SZ)
         ringBuf.next_ring_index = 0;

      if(ringBuf.ring_count < RCV_PACKET_BUFR_SZ)      // check if overflow -- should never happen!
         ringBuf.ring_count++;

      if(incomingData[0] == PAIRING_ACK)
         memcpy(&peerMac, macadr, 6);        // save peer mac adrs in pairing mode only
   }
   peer_connected = true;
}


/********************************************************************
 * @brief ESP-NOW send data callback.
 * 
 * @param mac_addr - 6 byte mac adrs of the target device
 * @param sendStatus - xmit good/bad
 */
void onDataSent(uint8_t *mac_addr, uint8_t sendStatus) 
{
   send_packet_status = sendStatus;
}


/********************************************************************
 * @brief Wait for esp_now send data callback to validate xmit.
 *
 * @note Routine times out after 10ms if callback does not get called.
 * @return true if callback has completed.
 */
bool checkXmit(void)
{
   bool ret = false;
   uint32_t looptmr = millis();
   uint8_t stat;

   while(true) 
   {
      noInterrupts();
         stat = send_packet_status;     // shared in data send callback
      interrupts();
      if(stat != SEND_WAITING_STATUS)
      {
         ret = (stat == SEND_OK);      // send successful if status == 0
         break;
      }
      yield();          // yield time for callbacks
      if(millis() - looptmr > 120)
         break;
   }
   return ret;
}


/********************************************************************
 * @brief Send data to peer. Retry if problems with xmit.
 * 
 * @param mac - mac address of target peer
 * @param src - pointer to source data
 * @param len - length of source data (not including packet header)
 * @param save_copy - if true, save a copy of the packet for possible resend.
 * @return true if transmit is successful
 */
bool sendData(uint8_t *mac, uint8_t *src, uint16_t len, bool save_copy)
{
   uint8_t i;
   bool ret = false;

   noInterrupts();
   send_packet_status = SEND_WAITING_STATUS;
   interrupts();

   esp_now_send(mac, src, len + PACKET_HDR_SIZE);     // send the packet data + header
   ret = checkXmit();      // wait for xmit complete
   if(ret)
   {
      /* Keep a copy in case the peer misses the packet */
      if(save_copy)
         memcpy((uint8_t *)&xmtMesgPacket_last, src, len + PACKET_HDR_SIZE);
   }
   return ret;
}


/********************************************************************
 * @brief SETUP()
 * 
 ********************************************************************/
void setup() 
{
   uint16_t i;
   uint32_t baudrate;
   pinMode(ONBOARD_LED, OUTPUT);          // LED is connected to GPIO0
   digitalWrite(ONBOARD_LED, LOW);

   ringBuf.cur_ring_index = 0;
   ringBuf.next_ring_index = 0;
   ringBuf.ring_count = 0;

   Serial.begin(SERIAL_BAUDRATE);
   while(!Serial) {}

   // initialize ESP-NOW Wifi
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();

   /* get the chip hardware mac addrs */
   WiFi.macAddress((uint8_t *)&ourMac);   // get default host mac

   /* initialize esp-now */
   if (esp_now_init() != 0) 
   {
      Serial.println("Failed to initialize ESP-NOW!");
   }
   else 
   {
      esp_now_set_self_role(ESP_NOW_ROLE_COMBO);      // role is both master & slave
      esp_now_register_recv_cb((esp_now_recv_cb_t) onDataRcv);    // register rcv callback
      esp_now_register_send_cb((esp_now_send_cb_t) onDataSent);   // register send callback
   }
   last_rcvd_packet_seq = 0;
   next_xmit_packet_seq = 0;
   peer_paired = false;
   peer_connected = false;

   resend_counter = 3;
   randomSeed(micros());         // try to make random more random
   pairing_interval = random(200, 400);
   pairing_timer = millis();

}     // ***** END SETUP() 


/********************************************************************
 * @brief LOOP()
 * 
 ********************************************************************/
void loop() 
{
   uint16_t i;
   /* 
    * Broadcast a pairing request periodically and wait for someone out there to respond...
    */
   if(millis() - pairing_timer > pairing_interval && !peer_paired)
   {
      pairing_timer = millis();
      digitalWrite(ONBOARD_LED, LOW);        // WiFi LED off
      xmtMesgPacket.packet_type = PAIRING_REQ;
      xmtMesgPacket.payload_len = 6;
      xmtMesgPacket.packet_seq = 0;
      xmtMesgPacket.mesg_filter = MESG_FILTER_KEY;

      memcpy((uint8_t *)&xmtMesgPacket.payload, (uint8_t *)&ourMac, 6);
      sendData((uint8_t *)&broadcastMac, (uint8_t *)&xmtMesgPacket,  6, false);
      pairing_interval = random(200, 500);
   }

   /* 
    * Process any data received from the peer 
    */
   if(ringBuf.ring_count > 0)
   {
      switch(ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].packet_type)
      {
         /* a peer has responded to a generic PAIRING REQUEST */
         case PAIRING_ACK:       
            /* Register the peer mac adrs if not already registered */
            if(!esp_now_is_peer_exist((uint8_t *)&peerMac))   // ignore if already registered
            {
               esp_now_add_peer((uint8_t *)&peerMac, ESP_NOW_ROLE_COMBO, 0, NULL, 0);  // register new peer mac
            }       
            peer_paired = true;     
            digitalWrite(ONBOARD_LED, HIGH);
            last_rcvd_packet_seq = 0;
            next_xmit_packet_seq = 0;
            resend_counter = 3;
            break;

         case PAIRING_REQ:
            /* A peer is attempting to pair with this node. Register peer Mac with its hardware Mac adrs */  
            memcpy(peerMac, ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload, 6);   // copy peer's mac adrs from their payload

            /* Register peer Mac if not already in the peer list */   
            if(!esp_now_is_peer_exist((uint8_t *)&peerMac))
            {
               esp_now_add_peer((uint8_t *)&peerMac, ESP_NOW_ROLE_COMBO, ESP_NOW_CHANNEL, NULL, 0);  
            }

            /* Acknowledge to peer that their mac is registered */
            xmtMesgPacket.packet_type = PAIRING_ACK;
            xmtMesgPacket.payload_len = 0;
            xmtMesgPacket.packet_seq = 0;
            xmtMesgPacket.mesg_filter = MESG_FILTER_KEY;
            sendData((uint8_t *)&peerMac,  (uint8_t *)&xmtMesgPacket, xmtMesgPacket.payload_len, false);
            break;   

         case DATA_ALL:
         case DATA_PARTIAL:
            /* A data packet has been received. Check if received packet is out of sequence. If so, 
             * request peer to resend last packet 
             */
            if(ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].packet_seq != last_rcvd_packet_seq +1)
            {
               if(resend_counter-- > 0)
               {
                  xmtMesgPacket.packet_type = DATA_RESEND_LAST;
                  xmtMesgPacket.payload_len = 0;
                  xmtMesgPacket.mesg_filter = MESG_FILTER_KEY;
                  last_rcvd_packet_seq = ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].packet_seq -1;    // expected packet seq # for resend 
                  xmtMesgPacket.packet_seq = last_rcvd_packet_seq;
                  sendData((uint8_t *)&peerMac, (uint8_t *)&xmtMesgPacket, xmtMesgPacket.payload_len, false);
               }
               else 
               {
                  /* Resend counter == 0, lost packet can't be recovered - move on */
#ifdef SERIAL_DATA_IS_BINARY                      
                  Serial.write((char *)ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload, ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload_len);
#else 
                  ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload[ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload_len] = 0x0;
                  Serial.print(ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload);
#endif                                    
                  last_rcvd_packet_seq = ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].packet_seq;
                  resend_counter = 3;
               }
            }
            else 
            {
               /* Transfer received data to the local Serial port */
#ifdef SERIAL_DATA_IS_BINARY                 
               Serial.write((char *)ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload, ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload_len); 
#else 
               ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload[ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload_len] = 0x0;
               Serial.print(ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].payload);
#endif
               /* Update the last received packet sequence number */
               last_rcvd_packet_seq = ringBuf.rcvMesgPacket[ringBuf.cur_ring_index].packet_seq;
            }
            break;

         case DATA_RESEND_LAST:
               sendData((uint8_t *)&peerMac, (uint8_t *)&xmtMesgPacket_last,  xmtMesgPacket_last.payload_len, false);
            break;

         case DATA_ACK:

            break;

         case KEEP_ALIVE_ACK:
 
            break;

         default:

            break;
      }
      if(++ringBuf.cur_ring_index >= RCV_PACKET_BUFR_SZ)
         ringBuf.cur_ring_index = 0;

      ringBuf.ring_count--;        // decr fifo item count
   }

   /* 
    * Check if Serial data is ready to be sent to the peer 
    */
   if(Serial.available() && peer_paired)    
   {
      uint8_t i = 0;
      xmtMesgPacket.packet_type = DATA_ALL;              // assume all data will fit in this packet
      while(Serial.available())
      {
         xmtMesgPacket.payload[i++] = Serial.read();     // read byte from serial buffer
         xmtMesgPacket.payload_len = i;
         if(i >= (MAX_PAYLOAD_SIZE))
         {
            if(Serial.available())
               xmtMesgPacket.packet_type = DATA_PARTIAL; // more data in the pipe - flag packet as partial
            
            xmtMesgPacket.payload_len = MAX_PAYLOAD_SIZE;
            break;
         }
         else 
         {
            xmtMesgPacket.payload[i] = 0x0;              // forward pad buffer with 0's
         }
      }

      /* Use new sequence number */
      xmtMesgPacket.packet_seq = next_xmit_packet_seq++;
      xmtMesgPacket.mesg_filter = MESG_FILTER_KEY;

      /* Send packet to most recent peer address. Returns true if xmit is OK */
      sendData((uint8_t *)&peerMac, (uint8_t *)&xmtMesgPacket, xmtMesgPacket.payload_len, true);
   }

   /* 
    * Check for lost comm. If lost, reset to periodic pairing request.
    */
   if(!peer_connected && peer_paired)
   {
      peer_paired = false;      // start new PAIRING scan
      pairing_timer = millis();
      pairing_interval = random(200, 500);
      digitalWrite(ONBOARD_LED, LOW);  // LED off if lost comm
   }
}        // ***** END LOOP()
