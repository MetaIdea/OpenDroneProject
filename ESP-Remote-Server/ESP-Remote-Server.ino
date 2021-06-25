//ESP-50E962
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include <ESP8266WiFiMulti.h>
#include <FS.h>   // Include the SPIFFS library
//#include "index.h"

#define ESP8266_BAUD_RATE 115200

const char* ssid = "";
const char* password = "";

String myIP = "";
String myPort = "888";

IPAddress drone_controller_ip;
unsigned int drone_controller_port;
bool drone_controller_registered = false;

ESP8266WebServer http_server(888);
WiFiServer server(888);

WiFiUDP UDP;
char packet[255];
unsigned int UDP_PORT = 888;

void BlinkLED() {
  digitalWrite(2, LOW);
  delay(50);
  digitalWrite(2, HIGH);
}

void handleRoot() {
  handleFileRead("/index.html");
  Serial.println(""); Serial.print("Website access");
  //BlinkLED();
}

void SendCommand(String CommandMessage) {
  Serial.println(""); Serial.print("Sending packet: "); Serial.print(CommandMessage);
  UDP.beginPacket(drone_controller_ip, drone_controller_port);
  UDP.write(CommandMessage.c_str());
  UDP.endPacket();
}

void SendMessageToDrone(String message) {
  if (message.indexOf("CMD:") >= 0) {
    if (message.indexOf("CMD:GPSWAYPOINTS:") >= 0) {
      SendCommand(message);
    }
    else if (message.indexOf("CMD:SETALTITUDE") >= 0) {
      SendCommand(message);
    }
    else if (message.indexOf("CMD:releaseDirection") >= 0) {
      SendCommand("S");
    }
    else if (message.indexOf("CMD:releaseAction") >= 0) {
      SendCommand("s");
    }
    else if (message.indexOf("CMD:connect") >= 0) {
      SendCommand("A");
    }
    else if (message.indexOf("CMD:reset") >= 0) {
      SendCommand("G");
    }
    else if (message.indexOf("CMD:start") >= 0) {
      SendCommand("C");
    }
    else if (message.indexOf("CMD:land") >= 0) {
      SendCommand("D");
    }
    else if (message.indexOf("CMD:emergencyshutdown") >= 0) {
      SendCommand("E");
    }
    else if (message.indexOf("CMD:calibrate") >= 0) {
      SendCommand("F");
    }
    else if (message.indexOf("CMD:front") >= 0) {
      SendCommand("I");
    }
    else if (message.indexOf("CMD:down") >= 0) {
      SendCommand("N");
    }
    else if (message.indexOf("CMD:left") >= 0) {
      SendCommand("K");
    }
    else if (message.indexOf("CMD:right") >= 0) {
      SendCommand("L");
    }
    else if (message.indexOf("CMD:rotateleft") >= 0) {
      SendCommand("M");
    }
    else if (message.indexOf("CMD:rotateright") >= 0) {
      SendCommand("O");
    }
    else if (message.indexOf("CMD:back") >= 0) {
      SendCommand("J");
    }
    else if (message.indexOf("CMD:up") >= 0) {
      SendCommand("P");
    }
    else if (message.indexOf("CMD:headless") >= 0) {
      SendCommand("X");
    }
    else if (message.indexOf("CMD:light") >= 0) {
      SendCommand("Z");
    }
    else if (message.indexOf("CMD:360turn") >= 0) {
      SendCommand("W");
    }
    else if (message.indexOf("CMD:fastreturn") >= 0) {
      SendCommand("Y");
    }
    else {
      SendCommand(message);
    }
  }
}

String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println(""); Serial.print("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.html";
  }
  String contentMIMEType = getContentType(path);
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t sent = http_server.streamFile(file, contentMIMEType);
    file.close();
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;
}

void startSPIFFS() {
  SPIFFS.begin(); // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) { // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s", fileName.c_str());
    }
    Serial.printf("\n");
  }
}

String DroneDataJson = "";
bool DroneDataGPSInit = false;

void setup(void){
  Serial.begin(ESP8266_BAUD_RATE);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  http_server.on("/", HTTP_GET, handleRoot);
  http_server.on("/", HTTP_POST, handleRoot);
  http_server.onNotFound([]() {
    if (String(http_server.uri()).indexOf("CMD:") >= 0) {
      SendMessageToDrone(http_server.uri());
      http_server.send(200);
    }
    else if (String(http_server.uri()).indexOf("DroneData") >= 0) {
      if (DroneDataGPSInit) {
        http_server.send(200, "application/json", DroneDataJson);
      } 
      else {
        http_server.send(200);
      }
    }
    else if (!handleFileRead(http_server.uri())) {
      http_server.send(404, "text/plain", "404: Not Found");
    }
  });
  http_server.begin();
  Serial.println("HTTP http_server started");

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);

  if (MDNS.begin("esp8266")) { // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  startSPIFFS(); // Start the SPI Flash Files System

}

void SendPacket(byte packet[], unsigned int packetSize) {
  UDP.beginPacket(drone_controller_ip, drone_controller_port);
  Serial.println("Sending packet: ");
  for (int i = 0; i < packetSize; i++) {
    UDP.write(packet[i]);
    Serial.print(" "); Serial.print(packet[i]);
  }
  UDP.endPacket();
}

bool DroneConnected = false;

void ReceiveUDPPacket() {
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    int len = UDP.read(packet, 255);
    if (len > 0) {
      packet[len] = '\0';
    }
    Serial.println(""); Serial.print("Packet received "); Serial.print("(size:"); Serial.print(packetSize); Serial.print("): "); Serial.print(packet); 
    if (String(packet).equals("RegisterDroneController")) {
      drone_controller_ip = UDP.remoteIP();
      drone_controller_port = UDP.remotePort();
      Serial.println(""); Serial.print("Drone Conrtoller IP: "); Serial.print(drone_controller_ip); Serial.print(" PORT:"); Serial.print(drone_controller_port);
      //drone_controller_registered = !drone_controller_registered;
      DroneConnected = true;
    } 
    else if (String(packet).indexOf("DATA:") >= 0) {
      String message = "" + String(packet);
      DroneDataJson = message.substring(5,message.length());
      DroneDataGPSInit = true;
      //http_server.send(200, "application/json", message.substring(6,message.length()-1));
    }
    else {
      Serial.println(""); Serial.print("Unknown UDP Message received "); Serial.print("(length:"); Serial.print(len); Serial.print("):"); Serial.print(packet);
    }
    //BlinkLED();
  }
}

String client_message = "";

void ReceiveWebsiteControlRequest() {
  WiFiClient client = server.available();
  if (client) {
    client_message = client.readStringUntil('\r');
    Serial.println(""); Serial.print("Client Message: "); Serial.print(client_message);
  }
}

int UDPKeepAliveTimeout = 10000; //30000 is regular max limit
int UDPTimeoutTimer = 0;

void KeepUDPConnectionAlive() {
  if ((UDPKeepAliveTimeout + UDPTimeoutTimer) <= millis()) {
    UDPTimeoutTimer = millis();
    SendCommand("abc");
  }
}

void loop(void){
  ReceiveWebsiteControlRequest();
  ReceiveUDPPacket();
  http_server.handleClient();
  if (DroneConnected) {
    KeepUDPConnectionAlive();
  }
  if (Serial.available()) {
    String usercmd = Serial.readStringUntil('\n');
    if (usercmd.equals("a")) {
      Serial.println(usercmd);
      http_server.send(200, "application/json", "{\"position\":{\"lat\":\"77\",\"lng\":\"77\"}}");
    }
  }
}
