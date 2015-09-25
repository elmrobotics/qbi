#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial SerialSoft(0,1);

#define LIGHT_PIN 5
#define FAN_PIN 6

int x;

void setup() {
  SerialSoft.begin(9600);
  SerialSoft.println("ActionHub Util");
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  x = 0;
  
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
}

void loop() {
  //If value received is 0 blink LED 1
  if (x == 0) {
   
  }
  //If value received is 1 blink LED 2
  if (x != 0) {
    SerialSoft.print("I got: ");
    SerialSoft.println(x);
    if (x == 11) {
      digitalWrite(LIGHT_PIN, HIGH);
    } else if (x == 12) {
      digitalWrite(LIGHT_PIN, LOW);
    } else if (x == 13) {
      digitalWrite(FAN_PIN, HIGH);
    } else if (x == 14) {
      digitalWrite(FAN_PIN, LOW);
    }
    x=0;
  }
}

void receiveEvent(int howMany) {
  x = Wire.read();    // receive byte as an integer
}
