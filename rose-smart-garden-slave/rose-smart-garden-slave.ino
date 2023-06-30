#include "soil-moisture-sens.h"
#include "output-module.h"
#include "sensor-module.h"
#include "input-module.h"
#include "serial-com.h"
#include "dht-sens.h"

#include "MemoryFree.h"

SensorModule sensor;
SerialCom com;

float dhtValueA[2];
float dhtValueB[2];
float soilValue[6];

void setup() {
        Serial.begin(9600);
}

void loop() {
        sensor.loop(subRoutine);
        com.clearData();
        for (uint8_t i = 0; i < 6; i++) {
                com.addData(soilValue[i]);
        }
        com.addData(dhtValueA[0]);
        com.addData(dhtValueA[1]);
        com.addData(dhtValueB[0]);
        com.addData(dhtValueB[1]);
        com.sendData();
        com.receive(onReceive);
        // debug();
}

void subRoutine() {
        sensor.addModule(new SoilMoistureSens(A0));
        sensor.addModule(new SoilMoistureSens(A1));
        sensor.addModule(new SoilMoistureSens(A2));
        sensor.addModule(new SoilMoistureSens(A3));
        sensor.addModule(new SoilMoistureSens(A4));
        sensor.addModule(new SoilMoistureSens(A5));
        sensor.addModule(new DHTSens(12));
        sensor.addModule(new DHTSens(13));

        sensor.init();
        sensor.update();

        for (uint8_t i = 0; i < 6; i++) {
                sensor.getModule(i)->getValue(&soilValue[i]);
        }
        sensor.getModule(6)->getValue(dhtValueA);
        sensor.getModule(7)->getValue(dhtValueB);
        sensor.clearModules();
}

void onReceive(String data) {
        
}

void debug() {
        Serial.print("| tempA: ");
        Serial.print(dhtValueA[0]);
        Serial.print("| humA: ");
        Serial.print(dhtValueA[1]);
        Serial.print("| tempB: ");
        Serial.print(dhtValueB[0]);
        Serial.print("| humB: ");
        Serial.print(dhtValueB[1]);

        Serial.print("| soilA: ");
        Serial.print(soilValue[0]);
        Serial.print("| soilB: ");
        Serial.print(soilValue[1]);
        Serial.print("| soilC: ");
        Serial.print(soilValue[2]);
        Serial.print("| soilD: ");
        Serial.print(soilValue[3]);
        Serial.print("| soilE: ");
        Serial.print(soilValue[4]);
        Serial.print("| soilF: ");
        Serial.print(soilValue[5]);

        Serial.print("| freeMemory()= ");
        Serial.print(freeMemory());
        Serial.println();
}
