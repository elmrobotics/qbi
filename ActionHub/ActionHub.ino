/* ActionHub - Qbi Appliance Control Server */
/* By: Elm Robotics - 2015 */
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiServer.h>
#include <LWiFiClient.h>

#define DEBUG false 

#define WIFI_AP "LinkItONEQbiB"
#define WIFI_PASSWORD "LinkItONE"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP according to your WiFi AP configuration

// URL Commands
#define LIGHT_ON "lighton"
#define LIGHT_OFF "lightoff"
#define FAN_ON "fanon"
#define FAN_OFF "fanoff"
#define QBI_UP "qbiup"
// On/Off Pins
#define LIGHT_PIN 12
#define FAN_PIN 11

LWiFiServer server(80);

void setup()
{  
  LTask.begin();
  LWiFi.begin();
  
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);

  Serial1.begin(9600); // LCD
  
#if DEBUG
  Serial.begin(9600);  // USB
  while (!Serial) {
    delay(100);
  }
#endif
          
  // keep retrying until connected to AP
#if DEBUG
  Serial.println("Connecting to AP ...");
#endif

  Serial1.printf("ActionHub\r\n");
  Serial1.printf("Connecting ...");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD))) {
    delay(1000);
  }

  printWifiStatus();
  
#if DEBUG
  Serial.println("Starting REST Server ...");
#endif

  server.begin();
  
#if DEBUG
  Serial.println("Server Started!");
#endif
}

void loop() {
  LWiFiClient client = server.available();
  long connectionStart = millis();
  if (client) {
#if DEBUG
    Serial.println("New Connection");
#endif
    boolean currentLineIsBlank = true;
    String currentline;
    String command;
    while (client.connected()) {
        if (client.available()) {
          char c = client.read();
#if DEBUG
          Serial.print(c);
#endif
          currentline += c;
          if (c == '\n' && currentLineIsBlank) {
#if DEBUG
            Serial.println("send response");
#endif
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/javascript");
            client.println("Connection: close");
            client.println();
            client.println("{ success: true }");
            client.println();
            if (command == LIGHT_ON){
              digitalWrite(LIGHT_PIN, HIGH);
              Serial1.print("ActionHub\r\n");
              Serial1.print("Light On");
            } else if (command == LIGHT_OFF) {
              digitalWrite(LIGHT_PIN, LOW);
              Serial1.print("ActionHub\r\n");
              Serial1.print("Light Off");
            } else if (command == FAN_ON) {
              digitalWrite(FAN_PIN, HIGH);
              Serial1.print("ActionHub\r\n");
              Serial1.print("Fan On");
            } else if (command == FAN_OFF) {
              digitalWrite(FAN_PIN, LOW);
              Serial1.println("ActionHub\r\n");
              Serial1.print("Fan Off");
            } else if (command == QBI_UP) {
              Serial1.print("ActionHub\r\n");
              Serial1.print("Qbi Up");
            }
            break;
          }
          if (c == '\n') {
            if (currentline.startsWith("GET /")) {
              command = currentline.substring(5, currentline.indexOf(' ', 5));
#if DEBUG
              Serial.println(command);
#endif
            }
            currentline = "";
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      delay(10);
      client.stop();
#if DEBUG
      Serial.println("Connection Closed");
#endif
   }
}

void printWifiStatus() {
  IPAddress ip = LWiFi.localIP();
  
  Serial1.printf("ActionHub Connected\r\n");
  Serial1.print(ip);
 
  
#if DEBUG
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
#endif
}
