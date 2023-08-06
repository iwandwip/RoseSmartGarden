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
        com.clearData();
        com.addData(firebase.getStrTime().c_str());
        com.sendData();
        com.receive(onReceive);
        debug();
}

void onReceive(String data) {
        // Serial.print(data);
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
                        firebase.clearData();
                        firebase.addData(soilValue[0], "/soil-moisture-0");
                        firebase.addData(soilValue[1], "/soil-moisture-1");
                        firebase.addData(soilValue[2], "/soil-moisture-2");
                        firebase.addData(soilValue[3], "/soil-moisture-3");
                        firebase.addData(soilValue[4], "/soil-moisture-4");
                        firebase.addData(soilValue[5], "/soil-moisture-5");

                        firebase.addData(dhtValueA[0], "/temperature-a");
                        firebase.addData(dhtValueA[1], "/humidity-a");
                        firebase.addData(dhtValueB[0], "/temperature-b");
                        firebase.addData(dhtValueB[1], "/humidity-b");
                        firebase.sendData(2000);
                        firebase.update();
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
