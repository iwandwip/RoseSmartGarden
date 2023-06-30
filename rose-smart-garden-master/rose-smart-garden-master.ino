#include "firebase-handler.h"
#include "serial-com.h"

FirebaseModule firebase;
SerialCom com;

float dhtValueA[2];
float dhtValueB[2];
float soilValue[6];

void setup() {
        Serial.begin(115200);
        firebase.connectToWiFi("KASTARA GROUP INDONESIA", "KASTARA@2022");
        firebase.waitConnection(1000);
        firebase.init();
}

void loop() {
        com.receive(onReceive);
        debug();
}

void onReceive(String data) {
        for (uint8_t i = 0; i < 6; i++) {
                soilValue[i] = com.getData(data, i);
        }
        dhtValueA[0] = com.getData(data, 6);
        dhtValueA[1] = com.getData(data, 7);
        dhtValueB[0] = com.getData(data, 8);
        dhtValueB[1] = com.getData(data, 9);
}

void serverHandler(void* pvParameter) {
        firebase.waitConnection(3000);
        for (;;) {
                if (firebase.isConnect()) {
                        // firebase.clearData();
                        // firebase.addData(value[0], "/voltage-in");
                        // firebase.addData(value[1], "/voltage-out");
                        // firebase.addData(value[2], "/current-in");
                        // firebase.addData(value[3], "/current-out");
                        // firebase.sendData(2000);
                }
                vTaskDelay(20 / portTICK_PERIOD_MS);
        }
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

        Serial.println();
}
