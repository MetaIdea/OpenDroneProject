#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include "A9G.h"

#define ESP8266_BAUD_RATE 115200

bool debugMode = false;
unsigned int sendRate = 50; //50 standard //even 1000ms works
unsigned int GPS_correction_rate = 2000;
bool ESP_AP_PLUS_SERVER_MODE = true;
bool AUTO_CONNECT_DRONE = false;
bool ConnectLTE4GModem = false;
bool UseSimCardAndSimUDPReceiver = true;
void CommandMessageLoop(bool RemoteServerMessage);

//DRONE WIFI INFO
const char* SSID_DRONE = "KY WIFI UFO-05BB3B9F";
const char* PASSWORD_DRONE = "";
IPAddress ip(192, 168, 169, 1);
unsigned int port = 8800;
bool DRONE_CONNECTED = false;

//ESP WIFI ACCESS POINT
const char* SSID_ACCESSPOINT = "ESP8266DroneController";
const char* PASSWORD_ACCESSPOINT = "";
bool AP_HIDDEN = false;
int AP_MAX_CON = 2;
IPAddress local_IP(192, 168, 8, 15); //local_IP(192, 168, 7, 7);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

//ESP SERVER
unsigned int localPort = 2228;
WiFiUDP UDP;
WiFiClient client;
WiFiServer server(localPort); //80
String  command_message = ""; // Command received from Android device

//4G MODEM WIFI INTERNET - OPTIONAl
const char* ssid_4G_Modem = "";
const char* password_4G_Modem = "";
bool LTE_MODEM_disconnected = true;

//REMOTE SERVER
String ip_remote_server_str = "";
String port_remote_server_str = "";
IPAddress ip_remote_server(0, 0, 0, 0);
unsigned int port_remote_server = 888;
bool drone_controller_registered = false;
#include "A9G.h"

void RegisterDroneControllerToRemoteServerWiFi() {
  UDP.beginPacket(ip_remote_server, port_remote_server);
  UDP.write("RegisterDroneController");
  UDP.endPacket();
  drone_controller_registered = !drone_controller_registered;
}

char packet[255];

String CheckReceiveUDPMessageWiFi() {
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    int len = UDP.read(packet, 255);
    if (len > 0) {
      packet[len] = '\0';
      command_message = packet;
      CommandMessageLoop(true);
    }
  }
}

void connect_drone() {
  WiFi.begin(SSID_DRONE, PASSWORD_DRONE);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println();
  Serial.print("Drone connected !");
  DRONE_CONNECTED = true;
}

void setup_wifi() {
  if (ESP_AP_PLUS_SERVER_MODE) {
    WiFi.mode(WIFI_AP_STA);
  } else {
    WiFi.mode(WIFI_STA);
  }
  if (ESP_AP_PLUS_SERVER_MODE) {
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(SSID_ACCESSPOINT, PASSWORD_ACCESSPOINT, AP_HIDDEN, AP_MAX_CON);
    server.begin();
    Serial.println("ESP8266 Local IP is: "); Serial.print((WiFi.localIP()));
    UDP.begin(localPort);
    Serial.printf("UDP Port %d\n", localPort);
  }
}

void ResetWiFi() { //to avoid auto reconnects even after flash
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}

void Connect4GModem() {
  WiFi.begin(ssid_4G_Modem, password_4G_Modem);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  LTE_MODEM_disconnected = false;
}

unsigned int lastDroneConnectionLoopTimeInMS = 0;
unsigned int lastGPSCorrectionLoopTimeInMS = 0;

void setup() {
  Serial.begin(ESP8266_BAUD_RATE);
  //Serial.setTimeout(80);
  Serial.println();
  ResetWiFi();
  setup_wifi();
  if (AUTO_CONNECT_DRONE) {
    connect_drone();
  }
  if (ConnectLTE4GModem) {
    Connect4GModem();
    RegisterDroneControllerToRemoteServerWiFi();
  }
  if (UseSimCardAndSimUDPReceiver) {
    SetupA9G();
    RegisterDroneControllerToRemoteServerSim();
  }
}

//ALTERNTIVE FOR THE WEBSITE GAMEPAD CONTROL IS THIS: GAMEPAD SERVER CODE - PLAYSTORE APP: ESP8266-Controller-plus-ULTRA
//SOURCE: https://github.com/engrpanda/ESP8266-Controller-plus-ULTRA/blob/master/ESP8266_sample_program/ESP8266_sample_program.ino

String checkClient (void) {
  while (!client.available()) delay(1);
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length() - 9, 9);
  return request;
}

void sendBackEcho(String echo) { /* send command echo back to android device */
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println(echo);
  client.println("</html>");
  client.stop();
  delay(1);
}

int up_down_sensibility = 64;
int rotation_sensibility = 127;
int moving_sensibility = 64;

String echo = "";
int TimeLastKeepAliveMessage = 0;

void CommandMessageLoop(bool RemoteServerMessage) {
  if (!RemoteServerMessage) {
    client = server.available();
    if (!client ) return;
    command_message = checkClient();
    echo = command_message;
  }
  //Serial.println(command_message);
  if (command_message == "abc") { //keep alive message
    TimeLastKeepAliveMessage = millis();
    Serial.println("");  Serial.println("KeepUDPConnectionAlliveMessage");
  }
  else if (command_message == "I") { //forward
    command_message = "front " + String(moving_sensibility);
    parseCommand(true);
  }
  else if (command_message == "J") { //DOWN
    command_message = "back " + String(moving_sensibility);
    parseCommand(true);
  }
  else if (command_message == "K") { //LEFT
    command_message = "left " + String(moving_sensibility);
    parseCommand(true);
  }
  else if (command_message == "L") { //RIGHT
    command_message = "right " + String(moving_sensibility);
    parseCommand(true);
  }
  else if (command_message == "M") { //SQUARE
    command_message = "rotateleft " + String(rotation_sensibility);
    parseCommand(true);
  }
  else if (command_message == "N") { //TRIANGLE
    command_message = "down " + String(up_down_sensibility);
    parseCommand(true);
  }
  else if (command_message == "O") { //CIRCLE
    command_message = "rotateright " + String(rotation_sensibility);
    parseCommand(true);
  }
  else if (command_message == "P") { //X
    command_message = "up " + String(up_down_sensibility);
    parseCommand(true);
  }
  else if (command_message == "C") { //L1
    start();
    //command_message = "fastfly";
    //parseCommand(true);
  }
  else if (command_message == "D") { //L2
    land();
    //command_message = "fastdrop";
    //parseCommand(true);
  }
  else if (command_message == "E") { //R1
    command_message = "emergencystop";
    parseCommand(true);
  }
  else if (command_message == "F") { //R2
    command_message = "calibrate";
    parseCommand(true);
  }
  else if (command_message == "A") { //S1 ON
    connect_drone();
  }
  else if (command_message == "B") { //S1 OFF
    DRONE_CONNECTED = false;
  }
  else if (command_message == "G") { //S2 ON
    command_message = "init";
    parseCommand(true);
  }
  else if (command_message == "H") { //S2 OFF
    command_message = "init";
    parseCommand(true);
  }
  else if (command_message == "S") { //DIRECTION BUTTON RELEASED
    restoreState();
  }
  else if (command_message == "s") { //ACTION BUTTON RELEASED
    restoreState();
  }
  else if (command_message == "W") {
    toggleCircleTurnEnd();
  }
  else if (command_message == "X") {
    toggleNoHeadMode();
  }
  else if (command_message == "Y") {
    toggleFastReturn();
  }
  else if (command_message == "Z") {
    toggleOpenLight();
  }
  else if (command_message.indexOf("CMD_GPSWAYPOINTS:") >= 0) {
    Serial.println("");  Serial.println(command_message);
  }
  else if (command_message.indexOf("CMD_ALTITUDE:") >= 0) {
    Serial.println("");  Serial.println(command_message);
  }
  else { //SLIDER [1-10]
    //int sensibility = (int)(command_message.toInt() * 12.7);
    //up_down_sensibility = sensibility;
    //rotation_sensibility  = sensibility;
    //moving_sensibility  = sensibility;
    Serial.println("");  Serial.print("Undetected command: "); Serial.print(command_message);
  }
  if (!RemoteServerMessage) {
    sendBackEcho(echo);
  }
  command_message = "";
}
///////////////////

byte mainPacket[]           = {0xEF, 0x02, 0x40, 0x00, 0x02, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x66, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x99};
byte diffPacket[]           = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte keepAlivePacket[]      = {0xEF, 0x02, 0x40, 0x00, 0x02, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x66, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x99};
byte actionMoveUpPacket[]   = {0xEF, 0x02, 0x3C, 0x00, 0x02, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x66, 0x80, 0x80, 0xD1, 0x80, 0x00, 0x51, 0x99};
byte actionKeepAlive[]      = {0x66, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x99};
byte actionEmergencyStop[]  = {0x66, 0x80, 0x80, 0x80, 0x80, 0x04, 0x04, 0x99};
byte actionGyroCorrection[] = {0x66, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x99};

//to test:
byte requestCTPSignature[]  = {0xEF, 0x00, 0x04, 0X00}; //???
//byte requestCTPSignature[]  = {0xEF, 0x02, 0x60, 0X00, 0x02, 0x02, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x65, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//byte requestCTPSignature[]  = {0xEF, 0x02, 0x28, 0X00, 0x02, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//EF 02 28 00 02 02 00 01 00 00 00 00 49 03 00 00 65 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
//EF 03 00 00 66 80 80 00 80 00 80 99

//control byte array explanation:
//byte0: 0x66 - startByte - byte 0x66, int 102
//byte1: 0x80 - 1-255 controlByte1/roll - aka left/right
//byte2: 0x80 - 1-255 controlByte2/pitch - aka forward/backward
//byte3: 0x00 - 1-255 controlAccelerator/throttle - aka speed
//byte4: 0x80 - 1-255 controlTurn/yaw - aka direction
//byte5: 0x00 - i/mode* e.g. FastFly, GyroCorrection, NoHeadMode, ...
//byte6: 0x80 - i2/checksum - validation number calculated from byte 1 to 5
//byte7: 0x99 - endByte - hex 0x99, int 153

int rollByte = 17; //x
int pitchByte = 18; //y
int throttleByte = 19; //z
int yawByte = 20; //rot
int modeByte = 21;
int checksumByte = 22;

//*ControlByte5/Mode (multiple modes -> additionally added to sum):
int NoFastFly = 0;
int FastFly = 1; //TakeOff/LaunchMode
int FastDrop = 2; //land
int EmergencyStop = 4;
int CircleTurnEnd = 8;
int NoHeadMode = 16;
int FastReturn = 32;
int UnLock = 32;
int OpenLight = 64;
int GyroCorrection = 128;
bool stateNoFastFly = true;
bool stateFastFly = false;
bool stateFastDrop = false;
bool stateEmergencyStop = false;
bool stateCircleTurnEnd = false;
bool stateNoHeadMode = false;
bool stateFastReturn = false;
bool stateUnLock = false;
bool stateOpenLight = false;
bool stateGyroCorrection = false;

unsigned int CTP_SIGNATURE = 0;
int MSG_CONNECT_CTP = 179; //0xB3

bool printPacket = false;
unsigned int lastKeyByte = throttleByte;

void setChecksum() {
  mainPacket[checksumByte] = ((((mainPacket[rollByte] ^ mainPacket[pitchByte]) ^ mainPacket[throttleByte]) ^ mainPacket[yawByte]) ^ (mainPacket[modeByte]) & 255);
}

void resetDrone() {
  mainPacket[2]             = 0x40;
  mainPacket[rollByte]      = actionKeepAlive[1];
  mainPacket[pitchByte]     = actionKeepAlive[2];
  mainPacket[throttleByte]  = actionKeepAlive[3];
  mainPacket[yawByte]       = actionKeepAlive[4];
  mainPacket[modeByte]      = actionKeepAlive[5];
  mainPacket[checksumByte]  = actionKeepAlive[6];
  setChecksum();
  for (int i = 0; i < 24; i++) {
    diffPacket[i] = 0;
  }
}

void restoreState() {
  if (lastKeyByte != -1) {
    mainPacket[lastKeyByte] = mainPacket[lastKeyByte] - diffPacket[lastKeyByte];
    diffPacket[lastKeyByte] = 0;
    setChecksum();
    lastKeyByte = -1;
  }
}

void moveUp(unsigned int throttle, unsigned int timeInMs) {
  //lastKeyByte = -1;
  lastKeyByte = throttleByte;
  mainPacket[throttleByte] = mainPacket[throttleByte] + throttle; //((255-128)/100)*throttle;  //173 takes off
  if (mainPacket[throttleByte] > 255) {
    mainPacket[throttleByte] = 255;
  }
  diffPacket[throttleByte] += throttle;
  mainPacket[2] = 0x3C;
  setChecksum();
}

void moveDown(unsigned int throttle, unsigned int timeInMs) {
  //lastKeyByte = -1;
  lastKeyByte = throttleByte;
  mainPacket[throttleByte] = mainPacket[throttleByte] - throttle;
  if (mainPacket[throttleByte] < 0) {
    mainPacket[throttleByte] = 0;
  } 
  else if (mainPacket[throttleByte] == 127) {
    mainPacket[throttleByte] = 128;
  }
  diffPacket[throttleByte] -= throttle;
  mainPacket[2] = 0x3C;
  setChecksum();
}

void moveLeft(unsigned int roll, unsigned int timeInMs) {
  lastKeyByte = rollByte;
  mainPacket[rollByte] = mainPacket[rollByte] - roll; //standard 168
  diffPacket[rollByte] -= roll;
  mainPacket[2] = 0x3C;
  setChecksum();
}

void moveRight(unsigned int roll, unsigned int timeInMs) {
  lastKeyByte = rollByte;
  mainPacket[rollByte] = mainPacket[rollByte] + roll;
  diffPacket[rollByte] += roll;
  mainPacket[2] = 0x3C;
  setChecksum();
}

void moveForward(unsigned int pitch, unsigned int timeInMs) {
  lastKeyByte = pitchByte;
  mainPacket[pitchByte] = mainPacket[pitchByte] + pitch; //standard 168
  diffPacket[pitchByte] += pitch;
  mainPacket[2] = 0x3C;
  setChecksum();
}

void moveBackward(unsigned int pitch, unsigned int timeInMs) {
  lastKeyByte = pitchByte;
  mainPacket[pitchByte] = mainPacket[pitchByte] - pitch;
  diffPacket[pitchByte] -= pitch;
  mainPacket[2] = 0x3C;
  setChecksum();
}

void rotateleft(unsigned int yaw, unsigned int timeInMs) {
  lastKeyByte = yawByte;
  mainPacket[yawByte] = mainPacket[yawByte] - yaw;
  diffPacket[yawByte] -= yaw;
  mainPacket[2] = 0x3C;
  setChecksum();
}

void rotateright(unsigned int yaw, unsigned int timeInMs) {
  lastKeyByte = yawByte;
  mainPacket[yawByte] = mainPacket[yawByte] + yaw;
  diffPacket[yawByte] += yaw;
  mainPacket[2] = 0x3C;
  setChecksum();
}

void start() {
  Serial.println("start drone");
  mainPacket[throttleByte] = 128;
  mainPacket[2] = 0x3C;
  EnableMode(stateFastFly, FastFly);
  printPacket = true;
  SendPacket((byte*)mainPacket, 24);
  //delay(sendRate);
}

void land() {
  Serial.println("land drone");
  mainPacket[throttleByte] = 0;
  mainPacket[2] = 0x3C;
  DisableMode(stateFastFly, FastFly);
  toggleMode(stateFastDrop, FastDrop);
  printPacket = true;
  SendPacket((byte*)mainPacket, 24);
  toggleMode(stateFastDrop, FastDrop);
  //delay(sendRate);
}

void toggleMode(bool &modeState, unsigned int modeByteNum) {
  if (!modeState) {
    mainPacket[modeByte] += modeByteNum;
  }
  else {
    mainPacket[modeByte] -= modeByteNum;
  }
  modeState = !modeState;
  mainPacket[2] = 0x3C;
  setChecksum();
}

void DisableMode(bool &modeState, unsigned int modeByteNum) {
  if (modeState) {
    mainPacket[modeByte] -= modeByteNum;
    modeState = false;
    mainPacket[2] = 0x3C;
    setChecksum();
  }
}

void EnableMode(bool &modeState, unsigned int modeByteNum) {
  if (!modeState) {
    mainPacket[modeByte] += modeByteNum;
    modeState = true;
    mainPacket[2] = 0x3C;
    setChecksum();
  }
}

void toggleFastfly() {
  toggleMode(stateFastFly, FastFly);
  SendPacket((byte*)mainPacket, 24);
  toggleMode(stateFastFly, FastFly);
  delay(sendRate);
}

void toggleFastDrop() {
  toggleMode(stateFastDrop, FastDrop);
  SendPacket((byte*)mainPacket, 24);
  toggleMode(stateFastDrop, FastDrop);
  delay(sendRate);
}

void toggleEmergencyStop() {
  //for (int i=0;i<3;i++){
    toggleMode(stateEmergencyStop, EmergencyStop);
    //SendPacket((byte*)mainPacket, 24);
    //toggleMode(stateGyroCorrection, GyroCorrection);
    //delay(sendRate);
  //}
}

void toggleCircleTurnEnd() {
  toggleMode(stateCircleTurnEnd, CircleTurnEnd);
}

void toggleNoHeadMode() {
  toggleMode(stateNoHeadMode, NoHeadMode);
}

void toggleFastReturn() {
  toggleMode(stateFastReturn, FastReturn);
}

void toggleUnLock() {
  toggleFastReturn();
  stateUnLock = !stateUnLock;
}

void toggleOpenLight() {
  toggleMode(stateOpenLight, OpenLight);
}

void toggleGyroCorrection() {
  toggleMode(stateGyroCorrection, GyroCorrection);
  SendPacket((byte*)mainPacket, 24);
  toggleMode(stateGyroCorrection, GyroCorrection);
  delay(sendRate);
}

void MoveToGPSPosition(float destination_latitude, float destination_longitude) {
  float current_latitude = 0;
  float current_longitude = 0;
  float compass_orientation = 0;
  //rotate until rotangle >= destangle or rotangle >= destangle (dir dependent)
  //set altitude
  //move forward
  //check rotangle == destangle all the time
  //arrive - hold postion
  //use PID controller here
  if (GPSInit) {
    //Serial.println(""); Serial.println("GPS: "); Serial.println(GetGPSPositionShort());
  }
}

float GetMetricLatitude(float latitude){
  return (111132.954 - 559.822 * cos(2*latitude) + 1.175 * cos(4*latitude));
}

float GetMetricLongitude(float longitude){
  return (111132.954 * cos(longitude));
}

float GetDistance(float lat1, float long1, float lat2, float long2) {
	lat1 = ((M_PI)/180)*lat1; 
	long1 = ((M_PI)/180)*long1;
	lat2 = ((M_PI)/180)*lat2; 
	long2 = ((M_PI)/180)*long2; 
	return 2*asin(sqrt(pow(sin((lat2-lat1)/2),2)+cos(lat1)*cos(lat2)*pow(sin((long2-long1)/2),2)))*6371000;
	//return sqrt(pow(GetMetricLatitude(latitude1) - GetMetricLatitude(latitude2), 2) + pow(GetMetricLongitude(longitude1) - GetMetricLongitude(longitude2),2));
}

float lastLatitude = 0;
float lastLongitude = 0;
float lastDistance = 0;
float currentLatitude = 0;
float currentLongitude = 0;
float currentDistance = 0;
float maxDistanceTolerance = 0;
int numberValuesForDistanceToleranceInit = 10;

bool GPS_FIXED = false;

void GetGPSPositionCoroutine() {
  String GPSPosition = GetGPSPosition();
  if (!(GPSPosition.indexOf("NOGPS") >= 0)) {
    Serial.println(""); Serial.print("GPS Position: "); Serial.print(GPSPosition);
    GPS_FIXED = true;
    String Latitude = GetLatitude(GPSPosition);
    String Longitude = GetLongitude(GPSPosition);
    Serial.println(""); Serial.print("GPS Position: "); Serial.print(GPSPosition);
    lastLatitude = currentLatitude;
    lastLongitude = currentLongitude;
    currentLatitude = atof(Latitude.c_str());
    currentLongitude = atof(Longitude.c_str());
    lastDistance = currentDistance;
    currentDistance = GetDistance(lastLatitude, lastLongitude, currentLatitude, currentLongitude);
    if (maxDistanceTolerance < currentDistance && lastLatitude != 0 && currentDistance < 10000) {
      maxDistanceTolerance = currentDistance;
      Serial.println(""); Serial.print("New max distance: "); Serial.print(String(maxDistanceTolerance));
    }
    Serial.println(""); Serial.print("Distance to last position: "); Serial.print(String(currentDistance)); Serial.print(" m");
  }
}

void SendPacket(byte packet[], unsigned int packetSize) { //error at 2553  after 255 7
  setChecksum();
  UDP.beginPacket(ip, port);
  for (int i = 0; i < packetSize; i++) {
    if (i == 12) {
      packet[i] = (byte)((CTP_SIGNATURE >> (8 * 0)) & 0xff);
    }
    else if (i == 13) {
      packet[i] = (byte)((CTP_SIGNATURE >> (8 * 1)) & 0xff);
    }
    UDP.write(packet[i]);
    if (debugMode || printPacket) {
      Serial.print(" ");
      Serial.print(packet[i]);
    }
  }
  UDP.endPacket();
  CTP_SIGNATURE += 1; //Capture Tree Protocol Signature
  if (CTP_SIGNATURE > 0xFFFF) {
    CTP_SIGNATURE = 0x0000;
  }
  if (printPacket) {
    Serial.println();
    printPacket = false;
  }
  if (debugMode) {
    Serial.println("");
    Serial.print("CTP-Signature: ");
    Serial.print(packet[12]);
    Serial.print(packet[13]);
    Serial.println("");
  }
  lastDroneConnectionLoopTimeInMS = millis();
}

String command;
int command_number;

void parseCommand(bool is_command_message) {
  if (is_command_message) {
    command = command_message;
    command_number = 0;
  } else {
    command = "";
    command_number = 0;
  }
  if (Serial.available() || is_command_message) {
    printPacket = true;
    if (!is_command_message) {
      command = Serial.readStringUntil('\n');
    }
    sscanf(command.c_str(), "%*[^0123456789]%d", &command_number); //Serial.parseInt();
    if (command.equals("init") || command.equals("idle")) {
      Serial.println("-> Idle - (Re-)Initializing.");
      resetDrone();
    }
    else if (command.equals("register")) {
      RegisterDroneControllerToRemoteServerWiFi();
    }
    else if (command.indexOf("up") >= 0) {
      if (command_number >= 0) { //command_number >= 0
        moveUp(command_number, 0.2);
        Serial.println(""); Serial.print("-> moving up - speed: "); Serial.print(command_number); Serial.print("%");
      }
    }
    else if (command.indexOf("down") >= 0) {
      if (command_number >= 0) {
        moveDown(command_number, 0.2);
        Serial.println(""); Serial.print("-> moving down - speed: "); Serial.print(command_number); Serial.print("%");
      }
    }
    else if (command.indexOf("rotateleft") >= 0) {
      if (command_number >= 0) {
        rotateleft(command_number, 0.2);
        Serial.println(""); Serial.print("-> rotate left: "); Serial.print(command_number); Serial.print("%");
      }
    }
    else if (command.indexOf("rotateright") >= 0) {
      if (command_number >= 0) {
        rotateright(command_number, 0.2);
        Serial.println(""); Serial.print("-> rotate right: "); Serial.print(command_number); Serial.print("%");
      }
    }
    else if (command.indexOf("left") >= 0 /*&& !(command.indexOf("rotateleft") >= 0)*/) {
      if (command_number >= 0) {
        moveLeft(command_number, 0.2);
        Serial.println(""); Serial.print("-> moving left - speed: "); Serial.print(command_number); Serial.print("%");
      }
    }
    else if (command.indexOf("right") >= 0 /*&& !(command.indexOf("rotateright") >= 0)*/) {
      if (command_number >= 0) {
        moveRight(command_number, 0.2);
        Serial.println(""); Serial.print("-> moving right - speed: "); Serial.print(command_number); Serial.print("%");
      }
    }
    else if (command.indexOf("front") >= 0) {
      if (command_number >= 0) {
        moveForward(command_number, 0.2);
        Serial.println(""); Serial.print("-> moving forward - speed: "); Serial.print(command_number); Serial.print("%");
      }
    }
    else if (command.indexOf("back") >= 0) {
      if (command_number >= 0) {
        moveBackward(command_number, 0.2);
        Serial.println(""); Serial.print("-> moving back - speed: "); Serial.print(command_number); Serial.print("%");
      }
    }
    else if (command.equals("fastfly") || command.equals("start") || command.equals("takeoff")) {
      toggleFastfly();
      Serial.println(""); Serial.print("-> FastFly: FastFly "); Serial.print(stateFastFly);
    }
    else if (command.equals("fastdrop") || command.equals("land") || command.equals("l")) {
      toggleFastDrop();
      Serial.println(""); Serial.print("-> FastDrop: "); Serial.print(stateFastDrop);
    }
    else if (command.equals("emergencystop") || command.equals("stop") || command.equals("s")) {
      toggleEmergencyStop();
      Serial.println(""); Serial.print("-> EmergencyStop: "); Serial.print(stateEmergencyStop);
    }
    else if (command.equals("circleturnend") || command.equals("roll")) {
      toggleCircleTurnEnd();
      Serial.println(""); Serial.print("-> CircleTurnEnd: "); Serial.print(stateCircleTurnEnd);
    }
    else if (command.equals("noheadmode") || command.equals("headless")) {
      toggleNoHeadMode();
      Serial.println(""); Serial.print("-> NoHeadMode: "); Serial.print(stateNoHeadMode);
    }
    else if (command.equals("fastreturn") || command.equals("return") || command.equals("unlock")) {
      toggleFastReturn();
      Serial.println(""); Serial.print("-> FastReturn: "); Serial.print(stateFastReturn);
    }
    else if (command.equals("openlight")) {
      toggleOpenLight();
      Serial.println(""); Serial.print("-> OpenLight: "); Serial.print(stateOpenLight);
    }
    else if (command.equals("gyrocorrection") || command.equals("calibrate")) {
      toggleGyroCorrection();
      Serial.println(""); Serial.print("-> GyroCorrection: "); Serial.print(stateGyroCorrection);
    }
    else if (command.equals("info")) {
      printPacket = true;
    }
    else if (command.equals("debug")) {
      debugMode = !debugMode;
    }
    else if (command.equals("setchecksum")) {
      setChecksum();
    }
    else if (command.equals("sensibility")) {
      up_down_sensibility = command_number;
      rotation_sensibility  = command_number;
      moving_sensibility  = command_number;
    }
    else {
      Serial.println("-> Invalid command.");
    }
    Serial.println(" ");
  }
}

void DroneConnectionLoop() {
  if (WiFi.status() == WL_CONNECTED) {
    parseCommand(/*is_command_message =*/false);
    SendPacket((byte*)mainPacket, 24);
  }
  else {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" connected");
    resetDrone(); //recheck later
  }
}

void HandleIncomingUDPMessages() {
  if (UseSimCardAndSimUDPReceiver) {
    CheckReceiveUDPMessageSIMHandler();
  }
  if (ConnectLTE4GModem) {
    CheckReceiveUDPMessageWiFi();
  }
}

void loop() {
  //LoopA9G();
  HandleIncomingUDPMessages();
  if (DRONE_CONNECTED && ((lastDroneConnectionLoopTimeInMS + sendRate) <= millis())) {
    DroneConnectionLoop();
  }
  if (GPSInit && ((lastGPSCorrectionLoopTimeInMS + GPS_correction_rate) <= millis())) {
    //GetGPSPositionCoroutine();
    lastGPSCorrectionLoopTimeInMS = millis();
  }
  if (UseSimCardAndSimUDPReceiver) {
    SendDroneDataToRemoteServer();
    //RegisterDroneControllerToRemoteServerSim();
  }
}

//todo: make the controller box having it's own loacal network html control website and use mdns esp lib to allow a handy url to access it
