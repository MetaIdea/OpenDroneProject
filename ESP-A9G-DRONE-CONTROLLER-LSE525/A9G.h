#include<stdio.h>
#include<string.h>
#include <SoftwareSerial.h>

#define BAUD_RATE 115200
#define A9_BAUD_RATE 115200

#define A9G_PON     16  //ESP12 GPIO16 A9/A9G POWON
#define A9G_POFF    15  //ESP12 GPIO15 A9/A9G POWOFF
#define A9G_WAKE    13  //ESP12 GPIO13 A9/A9G WAKE
#define A9G_LOWP    2  //ESP12 GPIO2 A9/A9G ENTER LOW POWER MODULE

SoftwareSerial SimSerial(14, 12, false, 256);

#define DEBUG true
bool InternetGPRSConnectionInit = false;
bool RemoteServerConnectionInit = false;
bool GSMInit = true; //these facilities are available on A9G power on automatically 
bool GPSInit = false;

String A9G_PORT = "2228";

void handleInterrupt() {
  Serial.println("An interrupt has occurred.");
}

String sendData(String command, const int timeout, boolean debug) {
  String response = "";    
  SimSerial.println(command); 
  long int time = millis();   
  while((time+timeout) > millis()) {
    yield();
    delay(0);
    while(SimSerial.available()) {       
      char c = SimSerial.read(); 
      response+=c;
    }  
  }    
  if(debug) {
    Serial.print(response);
  }    
  return response;
}

int A9GPOWERON() {
  digitalWrite(A9G_PON, LOW);
  delay(3000); //shiped with 3000
  digitalWrite(A9G_PON, HIGH);
  delay(5000); //shiped with 5000
  int timeout = 30000;
  String response = "";
  int response_length = 0;
  SimSerial.println("AT"); 
  long int time = millis();   
  while((time+timeout) > millis()) {
    yield();
    delay(0);
    while(SimSerial.available()) {
      char c = SimSerial.read();
      response+=c;
      response_length = response.length();
      if((response.indexOf("READY") >= 0) || (response_length >= 10/*140*/ && (response.substring(response_length-5,response_length)).indexOf("READY") >= 0)) {
        Serial.println("A9G POWER ON SUCCESSFUL");
        return 1;
      }
    }
  }
  Serial.println(response);
  Serial.println("A9G POWER ON FAILED");
  return 0;
}

int A9GPOWEROFF() { //needs rework like A9GPOWERON
  digitalWrite(A9G_POFF, HIGH);
  delay(3000);
  digitalWrite(A9G_POFF, LOW);
  delay(5000);
  String msg = String("");
  msg=sendData("AT",1000,DEBUG);
  if( msg.indexOf("OK") >= 0 ){
    Serial.println("A9G POWER OFF SUCCESSFUL");
    return 1;
  }
  else {
    Serial.println("A9G POWER OFF FAILED");
    return 0;
  }
}

int A9GENTERLOWPOWER() {
  String msg = String("");
  msg=sendData("AT+SLEEP=1",1000,DEBUG);
  if( msg.indexOf("OK") >= 0 ){
    digitalWrite(A9G_LOWP, LOW);
    return 1;
  }
  else {
     return 0;
  }
}

void SetupGSM() {
  if (!GSMInit) {
    Serial.println("Enabling mobile cell phone connection (GSM) via A9G (SIM).");
    Serial.println("Sim card status:");
    sendData("AT+CPIN?", 50, DEBUG); //sim card present
    Serial.println("-> Enable Network:"); 
    sendData("AT+CREG=1", 50, DEBUG); //network registration, 1,1 = connected
    Serial.println("Network registration status:");
    sendData("AT+CGREG?", 50, DEBUG); //check network registration
    Serial.println("Network info:");
    sendData("AT+COPS?", 50, DEBUG); //network info
    Serial.println("Signal quality:");
    sendData("AT+CSQ", 50, DEBUG); //signal quality
  }
}

void SetupGPS() {
  if (!GPSInit) {
    Serial.println("Enabling GPS positioning system via A9G");
    GPSInit = true;
    sendData("AT+GPS=1",1000,DEBUG);
  }
}

void SetupGPRS() {
  if (!InternetGPRSConnectionInit) {
    int timeout = 1000;
    Serial.println("Enabling Internet (GPRS) via A9G (SIM).");
    InternetGPRSConnectionInit = true;
    sendData("AT", timeout, DEBUG);
    sendData("AT+CGATT=1", timeout, DEBUG); //net attach
    sendData("AT+CGDCONT=1,IP,pinternet.interkom.de", timeout, DEBUG); //e.g. web.vodafone.de //pinternet.interkom.de
    sendData("AT+CGACT=1,1", timeout, DEBUG); //activate PDP
    sendData("AT+CIFSR", timeout, DEBUG);
    //sendData("AT+CIPMUX=1", 1000, DEBUG);
  }
}

void GetStringFromHex(String hex) {
  int len = hex.length();
  String newString;
  for(int i=0; i < len; i+=2) {
    String strbyte = hex.substring(i,2);
    char chr = (char) (int)strtol(strbyte.c_str(), NULL, 16);
    newString += chr;
  }
  Serial.println(newString);
 }

String QueryRemainingDataplanPhoneCreditLeft() {
  sendData("AT+CSCS=HEX", 0, DEBUG);
  //sendData("AT+CMGF=1", 0, DEBUG);
  String response = sendData("AT+CUSD=1,*104#,15", 5000, DEBUG);
  GetStringFromHex(response.substring(response.indexOf("\"")+1,response.indexOf(",")-2));
  Serial.println(""); Serial.print("Data left (MB): "); Serial.print(response);
  Serial.println(""); Serial.print("Credits left (€): "); Serial.print(sendData("AT+CUSD=1,*101#,15", 5000, DEBUG));
  return response;
}

void SetupA9G() {
  SimSerial.begin(A9_BAUD_RATE);

  for (char ch = ' '; ch <= 'z'; ch++) {
    SimSerial.write(ch);
  }
  SimSerial.println("");

  pinMode(A9G_PON, OUTPUT);//LOW LEVEL ACTIVE
  pinMode(A9G_POFF, OUTPUT);//HIGH LEVEL ACTIVE
  pinMode(A9G_LOWP, OUTPUT);//LOW LEVEL ACTIVE
  digitalWrite(A9G_PON, HIGH); 
  digitalWrite(A9G_POFF, LOW); 
  digitalWrite(A9G_LOWP, HIGH); 

  Serial.println("A9G POWERING ON - WAIT -");
  delay(2000);
  A9GPOWERON();
  pinMode(A9G_WAKE, INPUT); //interruptPin
  attachInterrupt(digitalPinToInterrupt(A9G_WAKE), handleInterrupt, RISING);
  
  SetupGSM();
  SetupGPS();
  SetupGPRS();

  //QueryRemainingDataplanPhoneCreditLeft();
}

void LoopA9G() {
  while (SimSerial.available() > 0) {
    Serial.write(SimSerial.read());
  }
  while (Serial.available() > 0) {
    SimSerial.write(Serial.read());
  }
}

String ExtractNumericSection(String content) {
  unsigned int startPos = 0;
  do {
    if (isdigit(content.charAt(startPos)))
      break;
  }
  while(++startPos < content.length());
  unsigned int endPos = content.length()-1;
  do {      
    if (isdigit(content.charAt(endPos)))
      break;
  }
  while(--endPos > startPos);
  return content.substring(startPos, endPos);
}

String GetGPSPosition(){
  String GPS = sendData("AT+LOCATION=2",3000,false);;
  if (!(GPS.indexOf("GPS NOT") >= 0) && !(GPS.indexOf("ERROR") >= 0)) {
    GPS = ExtractNumericSection(GPS.substring(16,35));
    if (GPS.length() >= 16) {
      return GPS;
    }
  }
  return "NOGPS";
}

String GetLatitude(String GPS) {
  return GPS.substring(0,GPS.indexOf(","));
}

String GetLongitude(String GPS) {
  return GPS.substring(GPS.indexOf(",")+1,GPS.length());
}

String GetGPSPosition2(){
  String GPS_position=sendData("AT+LOCATION=2",1000,DEBUG);
  if( GPS_position.indexOf("OK") >= 0 ){
    Serial.println("GET OK");
    String LG=GPS_position.substring(14,37);
    Serial.print("lg**********:");
    Serial.println(LG);  
   //SendTextMessage(LG,phonenum);
    return LG;
  }
  return "";
}

void SendTextMessage(String message, String phonenum) { 
  SimSerial.println("AT+CMGF=1");
  SimSerial.println("AT+CMGS=\"" + phonenum + "\"");
  SimSerial.println(message); 
  SimSerial.write(26);
}

void SendUDPMessage(String message, String remote_ip, String remote_port) {
  Serial.println(""); Serial.print("Sending UDP Message: "); Serial.print(message); Serial.println("");
  int timeout = 1000;
  if (!InternetGPRSConnectionInit) {
    InternetGPRSConnectionInit = true;
    sendData("AT", timeout, DEBUG);
    sendData("AT+CGATT=1", timeout, DEBUG); //net attach
    sendData("AT+CGDCONT=1,IP,pinternet.interkom.de", timeout, DEBUG); //O2
    //sendData("AT+CGDCONT=1,IP,pinternet.interkom.de", timeout, DEBUG); //NETZCLUB
    sendData("AT+CGACT=1,1", timeout, DEBUG); //activate PDP
    sendData("AT+CIFSR", timeout, DEBUG); //show my ip
    //sendData("AT+CIPMUX=1", 1000, DEBUG); //Enable SIM/GPRS server multi connection
    //sendData("AT+CIPSTATUS", 1000, DEBUG); //check internet registration, Initial = connected
  }
  if (!RemoteServerConnectionInit) {
    RemoteServerConnectionInit = true;
    sendData(String("AT+CIPSTART=UDP,") + remote_ip + String(",") + remote_port, timeout, DEBUG);
  }
  sendData("AT+CIPSEND", timeout, DEBUG);
  SimSerial.print(message); 
  SimSerial.write(26);
  //sendData(String("AT+CIPCLOSE=0"), timeout, DEBUG);
}

String CheckReceiveUDPMessageSim() {
  String response = "";
  if (SimSerial.available()) {
    while(SimSerial.available()) {
      char c = SimSerial.read();
      response+=c;
    }
    //response += '\0';
  }
  return response;
}

void CheckReceiveUDPMessageSIMHandler() {
  String SimUDPMessage =  "" + CheckReceiveUDPMessageSim();
  int MsgLength = SimUDPMessage.length();
  if (MsgLength > 0) {
    if ((SimUDPMessage.indexOf("+CIPRCV") >= 0)) {
      if (SimUDPMessage.indexOf("+CIPRCV,1:") >= 0) {
        command_message = SimUDPMessage.substring(10, 11);
      } 
      else {
        command_message = SimUDPMessage.substring(SimUDPMessage.indexOf(":") + 1, MsgLength-1);
      }
      CommandMessageLoop(true);
    } 
    else {
      Serial.println(""); Serial.print("Unknown UDP Message received "); Serial.print("(length:"); Serial.print(MsgLength); Serial.print("):"); Serial.print(command_message);
    }
  }
}

void RegisterDroneControllerToRemoteServerSim() {
  SendUDPMessage("RegisterDroneController", ip_remote_server_str, String(port_remote_server));
}

int UDPKeepAliveTimeout = 10000; //30000 is regular max limit
int UDPTimeoutTimer = 0;

int RemoteServerDroneDataSendRate = 3000; //at least 30000 is necessary to maintain UDP connection
int RemoteServerDroneDataTimeoutTimer = 0;

void SendDroneDataToRemoteServer() {
  if ((RemoteServerDroneDataSendRate + RemoteServerDroneDataTimeoutTimer) <= millis()) {
    RemoteServerDroneDataTimeoutTimer = millis();
    String GPSPosition = GetGPSPosition();
    if (!(GPSPosition.indexOf("NOGPS") >= 0)) {
      Serial.println(""); Serial.print("GPS Position: "); Serial.print(GPSPosition);
      String DroneData = String("DATA:") + String("{\"position\":{\"lat\":\"") + GetLatitude(GPSPosition) + String("\",\"lng\":\"") + GetLongitude(GPSPosition) + String("\"}}");
      SendUDPMessage(DroneData, ip_remote_server_str, String(port_remote_server));       
    }
  } 
  if ((UDPKeepAliveTimeout + UDPTimeoutTimer) <= millis()) {
    SendUDPMessage("abc", ip_remote_server_str, String(port_remote_server));
    UDPTimeoutTimer = millis();
  }
}
