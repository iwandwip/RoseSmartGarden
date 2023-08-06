#include "SoftwareSerial.h"

#define SerialEsp espSerial

/*
3 2 16 17
3 2 17 16
2 3 16 17
2 3 17 16
*/

SoftwareSerial espSerial(3, 2);
String dataSend = "10;20;30;40";

void setup() {
        Serial.begin(9600);
        SerialEsp.begin(115200);
}

void loop() {
        // Serial.println(SerialEsp.available());
        if (SerialEsp.available() > 0) {
                Serial.print(SerialEsp.readString());
        }
        SerialEsp.print(dataSend);
        SerialEsp.println();
        delay(1000);
}