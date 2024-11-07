/*
	Secured Esp32 Websockets Client

	This sketch:
        1. Connects to a WiFi network
        2. Connects to a Websockets server (using WSS)
        3. Sends the websockets server a message ("Hello Server")
        4. Sends the websocket server a "ping"
        5. Prints all incoming messages while the connection is open

    NOTE:
    The sketch dosen't check or indicate about errors while connecting to 
    WiFi or to the websockets server. For full example you might want 
    to try the example named "Esp32-Client" (And use the ssl methods).

	Hardware:
        For this sketch you only need an Esp32 board.

	Created 15/02/2019
	By Gil Maimon
	https://github.com/gilmaimon/ArduinoWebsockets

*/

#include <ArduinoWebsockets.h>
#include <WiFi.h>

// update interval is 10 seconds in this example
#define UPDATE_INTERVAL (1000 * 10)
#define MESSAGE_MAX_SIZE 64

void connectWebSocketServer();

const char* ssid = ""; //Enter SSID
const char* password = ""; //Enter Password
int last_update_sent = 0;
int water_level = 0;
int food_level = 0;

const char* websockets_connection_string = "ws://0.tcp.sa.ngrok.io:18710"; //Enter server adress

using namespace websockets;
WebsocketsClient client;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
    Serial.println("onEventsCallback");
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
        client = {};
        delay(1000);
        connectWebSocketServer();
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void connectWebSocketServer() {
    // run callback when messages are received
    client.onMessage(onMessageCallback);
    
    // run callback when events are occuring
    client.onEvent(onEventsCallback);

    // Before connecting, set the ssl fingerprint of the server
    //client.setCACert(echo_org_ssl_ca_cert);

    // Connect to server
    client.connect(websockets_connection_string);

    // Send a message
    Serial.println("Sending message");
    client.send("Hello Server");

    // Send a ping
    //client.ping();
    //Serial.println("Sending ping");
}

void setup() {
    Serial.begin(115200);
    // Connect to wifi
    WiFi.begin(ssid, password);

    // Wait some time to connect to wifi
    Serial.println("Connecting to WiFi");
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("Connected to WiFi");

    connectWebSocketServer();
}

unsigned int getWaterLevel() {
  unsigned int water_level;

  // TODO: Leer el sensor, obtener el resultado y convertirlo a %
  // Borrar esta parte que simula el sensor
  water_level = random(0, 100);
  // Hasta aca

  return water_level;
}

unsigned int getFoodLevel() {
  unsigned int food_level;

  // TODO: Leer el sensor, obtener el resultado y convertirlo a %
  // Borrar esta parte que simula el sensor
  food_level = random(0, 100);
  // Hasta aca

  return food_level;
}

void loop() {
  unsigned int water_level, food_level; 
  char message[MESSAGE_MAX_SIZE];

  if (millis() - last_update_sent > UPDATE_INTERVAL) {
    // Chequear valores de agua y comida
    water_level = getWaterLevel();
    food_level = getWaterLevel();

    sprintf(message, "Water: %d", water_level);
    client.send(message);
    sprintf(message, "Food: %d", food_level);
    client.send(message);

    last_update_sent = millis();
  }
  client.poll();
}
