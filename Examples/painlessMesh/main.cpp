//*******************************************************************
// This is a simple example that uses the easyMesh library
//
// 1. blinks led once for every node on the mesh
// 2. blink cycle repeats every BLINK_PERIOD
// 3. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 4. prints anything it receives to the Serial port
//
//
//*******************************************************************
#include "painlessMesh.h"

/* GPIO pin that is connected to an LED */
#define   LED             2       // GPIO of connected LED, ON ESP-12F it's GPIO2

#define   BLINK_PERIOD    3000    // milliseconds until cycle repeat
#define   BLINK_DURATION  100     // milliseconds LED is on for

/** NOTE: Mesh SSID & Password must be the same on all participating nodes */
/** IMPORTANT: Password must be a minimum of 8 characters in length */
#define   MESH_SSID       "AMESH"       // name of mesh network
#define   MESH_PASSWORD   "12345678"    // *** Must be at least 8 chars
#define   MESH_PORT       5555

/* Function Prototypes */
void sendMessage(); 
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

/* instanciate libraries */
Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

/* Create task to send a message to all nodes every 1 - 5 seconds */
Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, &sendMessage ); // start with a one second interval

/* Task to blink the number of nodes */
Task blinkNoNodes;
bool onFlag = false;

/********************************************************************
 * @brief setup()
 * 
 */
void setup() {
   Serial.begin(115200);

   pinMode(0, OUTPUT);
   digitalWrite(0, LOW);
   pinMode(LED, OUTPUT);

   mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages

   mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
   mesh.onReceive(&receivedCallback);
   mesh.onNewConnection(&newConnectionCallback);
   mesh.onChangedConnections(&changedConnectionCallback);
   mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
   mesh.onNodeDelayReceived(&delayReceivedCallback);

   userScheduler.addTask( taskSendMessage );
   taskSendMessage.enable();

   blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() 
   {
      /* If on, switch off, else switch on */
      if (onFlag)
        onFlag = false;
      else
        onFlag = true;
      blinkNoNodes.delay(BLINK_DURATION);

      if (blinkNoNodes.isLastIteration()) 
      {
        /* Finished blinking. Reset task for next run */
        /* blink number of nodes (including this node) times */
        blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
        /* Calculate delay based on current mesh time and BLINK_PERIOD */
        /* This results in blinks between nodes being synced */
        blinkNoNodes.enableDelayed(BLINK_PERIOD - 
            (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
      }
   });
   userScheduler.addTask(blinkNoNodes);
   blinkNoNodes.enable();

   randomSeed(analogRead(A0));    // use white noise as entropy source
}


/********************************************************************
 * @brief loop()
 * 
 */
void loop() 
{
   mesh.update();
   digitalWrite(LED, !onFlag);
}

/********************************************************************
 * @brief send a message to the mesh...
 * 
 */
void sendMessage() {
  String msg = "Msg from node ID ";
  msg += mesh.getNodeId();
  // msg += " myFreeMemory: " + String(ESP.getFreeHeap());
  mesh.sendBroadcast(msg);

  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }

  //Serial.printf("Sending message: %s\n", msg.c_str());  // print when message is sent
  
  taskSendMessage.setInterval( random(TASK_SECOND * 1, TASK_SECOND * 5));  // between 1 and 5 seconds
}


/********************************************************************
 * @brief Print any message received from peers
 * 
 * @param from - the chip ID of the sender
 * @param msg - the text message from the sender
 */
void receivedCallback(uint32_t from, String & msg) 
{
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}


/********************************************************************
 * @brief Callback to notify of a new node connecting.
 * 
 * @param nodeId - chip ID of the new node.
 */
void newConnectionCallback(uint32_t nodeId) {
  /* Reset blink task */
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}


/********************************************************************
 * @brief Callback to notify a change in connections.
 * 
 */
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  /*  Reset blink task */
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}


/********************************************************************
 * @brief Sync time between nodes.
 * 
 * @param offset 
 */
void nodeTimeAdjustedCallback(int32_t offset) 
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}


/********************************************************************
 * @brief Measure and print message delay time in microseconds.
 * 
 * @param from - chip ID of sender
 * @param delay - delay time in microseconds
 */
void delayReceivedCallback(uint32_t from, int32_t delay) 
{
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}