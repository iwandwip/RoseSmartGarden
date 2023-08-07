#include "soil-moisture-sens.h"
#include "LiquidCrystal_I2C.h"
#include "output-module.h"
#include "sensor-module.h"
#include "input-module.h"
#include "serial-com.h"
#include "fiz-fuzzy.h"
#include "dht-sens.h"

#include "MemoryFree.h"

#define BTS1_ENA_PIN 7
#define BTS1_LPWM_PIN 8
#define BTS1_RPWM_PIN 9

#define BTS2_ENA_PIN 4
#define BTS2_LPWM_PIN 5
#define BTS2_RPWM_PIN 6

#define DHT_TEMPERATURE_A 0
#define DHT_HUMIDITY_A 1

#define DHT_TEMPERATURE_B 0
#define DHT_HUMIDITY_B 1

LiquidCrystal_I2C lcd(0x27, 16, 2);

SensorModule sensor;
SerialCom com;

DigitalOut fertilizerPump(25, true);
DigitalOut dhtPump(27, true);
DigitalOut moisturePump(29, true);

float dhtValueA[2];
float dhtValueB[2];
float soilValue[6];
float soilAverage;
String soilStatus;

const int fis_gcI = 2;
const int fis_gcO = 1;
const int fis_gcR = 9;

FIS_TYPE g_fisInput[fis_gcI];
FIS_TYPE g_fisOutput[fis_gcO];
FIS_TYPE g_fisOutputPWM;
String g_fisOutputStatus;

void setup() {
        Serial.begin(9600);
        lcd.init();
        lcd.backlight();
        pinMode(BTS1_ENA_PIN, OUTPUT);
        pinMode(BTS1_LPWM_PIN, OUTPUT);
        pinMode(BTS1_RPWM_PIN, OUTPUT);
        digitalWrite(BTS1_ENA_PIN, HIGH);
}

void loop() {
        // if (Serial.available() > 0) {
        //         String serialData = Serial.readString();
        //         dhtValueA[DHT_TEMPERATURE_A] = com.getData(serialData, 0);
        //         dhtValueA[DHT_HUMIDITY_A] = com.getData(serialData, 1);
        //         // soilValue[5] = com.getData(serialData, 0);
        // }

        sensor.loop(sensorRoutine);
        com.clearData();
        for (uint8_t i = 0; i < 6; i++) {
                com.addData(soilValue[i]);
        }
        com.addData(dhtValueA[DHT_TEMPERATURE_A]);
        com.addData(dhtValueA[DHT_HUMIDITY_A]);
        com.addData(dhtValueB[DHT_TEMPERATURE_B]);
        com.addData(dhtValueB[DHT_HUMIDITY_B]);
        com.sendData();
        com.receive(onReceive);

        debug();

        g_fisInput[0] = dhtValueA[DHT_HUMIDITY_A];     // Read Input: Kelembaban
        g_fisInput[1] = dhtValueA[DHT_TEMPERATURE_A];  // Read Input: Suhu
        g_fisOutput[0] = 0;
        fis_evaluate();
        g_fisOutputPWM = map(g_fisOutput[0], 0, 100, 0, 255);  // Set output value: Kipas
        analogWrite(BTS1_RPWM_PIN, 0);                         // g_fisOutputPWM
        analogWrite(BTS1_LPWM_PIN, 0);

        soilAverage = 0;
        for (uint8_t i = 0; i < 6; i++) {
                soilAverage += soilValue[i];
        }
        soilAverage /= 6;
        if (soilValue[0] < 50) {  // soilAverage
                moisturePump.on();
        } else {
                moisturePump.off();
        }
        if (dhtValueA[DHT_TEMPERATURE_A] > 28) {
                dhtPump.on();
        } else {
                dhtPump.off();
        }

        // fertilizerPump.on();

        char strUpperDisplay[20], strLowerDisplay[20];
        char strMoistureValue[20], strTemperatureValue[20];

        dtostrf(dhtValueA[DHT_TEMPERATURE_A], 2, 2, strTemperatureValue);
        dtostrf(dhtValueA[DHT_HUMIDITY_A], 2, 2, strMoistureValue);

        sprintf(strUpperDisplay, "Suhu: %s *C ", strTemperatureValue);
        sprintf(strLowerDisplay, "Kel : %s %% ", strMoistureValue);

        lcd.setCursor(0, 0);
        lcd.print(strUpperDisplay);
        lcd.setCursor(0, 1);
        lcd.print(strLowerDisplay);
}

void sensorRoutine() {
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

        if (soilValue[5] < 40) soilStatus = "Dry";
        else if (soilValue[5] >= 40 && soilValue[5] < 70) soilStatus = "Normal";
        else soilStatus = "Wet";
}

void onReceive(String data) {
        Serial.println(data);
}

void debug() {
        Serial.print("| temp: ");
        Serial.print(dhtValueA[DHT_TEMPERATURE_A]);
        Serial.print("| hum: ");
        Serial.print(dhtValueA[DHT_HUMIDITY_A]);

        // Serial.print("| tempB: ");
        // Serial.print(dhtValueB[DHT_TEMPERATURE_B]);
        // Serial.print("| humB: ");
        // Serial.print(dhtValueB[DHT_HUMIDITY_B]);

        Serial.print("| fisOut: ");
        Serial.print(g_fisOutput[0]);
        // Serial.print("| fisStatus: ");
        // Serial.print(g_fisOutputStatus);
        Serial.print("| fisPWM: ");
        Serial.print(g_fisOutputPWM);

        Serial.print("| avg: ");
        Serial.print(soilAverage);
        Serial.print("| sA: ");
        Serial.print(soilValue[0]);
        Serial.print("| sB: ");
        Serial.print(soilValue[1]);
        Serial.print("| sC: ");
        Serial.print(soilValue[2]);
        Serial.print("| sD: ");
        Serial.print(soilValue[3]);
        Serial.print("| sE: ");
        Serial.print(soilValue[4]);
        Serial.print("| sF: ");
        Serial.print(soilValue[5]);

        // Serial.print("| SoilValue: ");
        // Serial.print(soilValue[5]);
        // Serial.print("| SoilStatus: ");
        // Serial.print(soilStatus);

        // Serial.print("| freeMemory()= ");
        // Serial.print(freeMemory());
        Serial.println();
}
