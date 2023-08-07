#include "HardwareSerial.h"

#define SerialMega Serial
 
String dataSend = "50;60;70;80";

void setup() {
        // Serial.begin(115200);
        // SerialMega.begin(115200);
        SerialMega.begin(115200, SERIAL_8N1, 16, 17);
}

void loop() {
        // Serial.println(SerialMega.available());
        if (SerialMega.available() > 0) {
                Serial.print(SerialMega.readString());
        }
        SerialMega.print(dataSend);
        SerialMega.println();
        delay(1000);
}