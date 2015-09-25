#include <Suli.h>
#include <Wire.h>
#include "acc_adxl345_Arduino.h"
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>

// Qbi Sides
#define S_FANON 1
#define S_FANOFF 2
#define S_LIGHTON 3
#define S_LIGHTOFF 4
#define S_QBIUP 5
#define S_QBIDOWN 0

// Qbi LED GROUP
#define LED_FANON 3
#define LED_FANOFF 1
#define LED_LIGHTON 4
#define LED_LIGHTOFF 2
#define LED_QBIUP 5
#define LED_QBIDOWN 5

// Qbi ActionHub Commands
#define CMD_FANON "fanon"
#define CMD_FANOFF "fanoff"
#define CMD_LIGHTON "lighton"
#define CMD_LIGHTOFF "lightoff"
#define CMD_QBIUP "qbiup"
#define CMD_QBIDOWN "qbiup"

// WiFi
#define WIFI_AP "LinkItONEQbiA"
#define WIFI_PASSWORD "LinkItONE"
#define WIFI_AUTH LWIFI_WPA    // LWIFI_OPEN, LWIFI_WPA, LWIFI_WEP
#define ACTIONHUB_URL "192.168.1.43"


// Accelerometer
Acc_Adxl345 acc;
int led = 13;
float n1 = .5;
float n2 = .2;
int currentFace = 0;

int axisReadBuf = 800;
int lastReadTime = 0;
int lastReadFace = 0;

LWiFiClient client;

void setup() {
    // Wifi!
    LTask.begin();
    LWiFi.begin();
  
    Serial.begin(9600);
//    while (!Serial) {
//      delay(100);
//    }
    Serial.println("Hi, I'm Qbi :)");
    
    Serial.println("Starting Wire ...");
    Wire.begin();
    
    Serial.println("Starting Accelerometer ...");
    acc.begin();
    

    // keep retrying until connected to AP
    Serial.println("Connecting to AP");
    while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
    {
      delay(1000);
    }
    Serial.println("Connected to WiFi");
    printWifiStatus();
}

void loop() {
  // Web Response
  while (client.available()) {
    int v = client.read();
    if (v != -1){
      Serial.print((char)v);
    } else {
      Serial.println("No more content, disconnect");
      client.stop();
    }
  }
  
  // Face Detection
  float ax, ay, az;
  int readFace = 100;
  int lastFace;
  int startChangeTime = 0;
  acc.get(&ax,&ay,&az);             // get value from accelemeter
  if (ax < -n1 && (abs(ay) + abs(az)) < n2) {        // X Up
    readFace = S_FANON;
  } else if (ax > n1 && (abs(ay) + abs(az)) < n2) {  // X Down
    readFace = S_FANOFF;
  } else if (ay < -n1 && (abs(az) + abs(ax)) < n2) { // Y Up
    readFace = S_LIGHTOFF;
  } else if (ay > n1 && (abs(az) + abs(ax)) < n2) {  // Y Down
    readFace = S_LIGHTON;
  } else if (az < -n1 && (abs(ay) + abs(ax)) < n2) { // Z Up
    readFace = S_QBIDOWN;
  } else if (az > n1 && (abs(ay) + abs(ax)) < n2) {  // Z Down
    readFace = S_QBIUP;
  }
  
  if(readFace <= 6) {
    if(lastReadFace != readFace && readFace != currentFace) {
      lastReadTime = millis();
      lastReadFace = readFace;
    } else if ((lastReadTime +  axisReadBuf) < millis() && lastReadFace == readFace && readFace != currentFace) {
      currentFace = readFace;
      logFaceChange();
    }
  }
}

void  logFaceChange() {
  
    Serial.print("FACE: ");
    Serial.println(currentFace);
    if (currentFace < 10) {
     
      // Send Command to ActionHub
      String command = CMD_QBIUP;
      byte ledcmd = 0;
      if (currentFace == S_FANON) {
        command = CMD_FANON;
        ledcmd = LED_FANON;
      } else if (currentFace == S_FANOFF) {
        command = CMD_FANOFF;
        ledcmd = LED_FANOFF;
      } if (currentFace == S_LIGHTON) {
        command = CMD_LIGHTON;
        ledcmd = LED_LIGHTON;
      }  else if (currentFace == S_LIGHTOFF) {
        command = CMD_LIGHTOFF;
        ledcmd = LED_LIGHTOFF;
      } if (currentFace == S_QBIUP) {
        command = CMD_QBIUP;
        ledcmd = LED_QBIUP;
      }  else if (currentFace == S_QBIDOWN) {
        command = CMD_QBIDOWN;
        ledcmd = LED_QBIDOWN;
      }
      
      Serial.print("Sending to QbiUtil: ");
      Serial.println(command);
      Wire.beginTransmission(9);     // transmit to device #9
      Wire.write(ledcmd);
      Wire.endTransmission();        // stop transmitting
      
      Serial.print("Sending to ActionHub: ");
      Serial.println(command);
      sendCommandToActionHub(command);
    }
} 

void sendCommandToActionHub(String cmd) {
  Serial.print("Connectiong to ActionHub ");
  while (0 == client.connect(ACTIONHUB_URL , 80)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  
  // send HTTP request
  Serial.println("send HTTP GET request");
  client.print("GET /");
  client.print(cmd);
  client.println(" HTTP/1.1");
  client.println("Host: " ACTIONHUB_URL);
  client.println("Connection: close");
  client.println();
}

void printWifiStatus() {
  IPAddress ip = LWiFi.localIP();\
  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(LWiFi.SSID());

  // print your WiFi shield's IP address:
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.print("subnet mask: ");
  Serial.println(LWiFi.subnetMask());

  Serial.print("gateway IP: ");
  Serial.println(LWiFi.gatewayIP());

  // print the received signal strength:
  long rssi = LWiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
