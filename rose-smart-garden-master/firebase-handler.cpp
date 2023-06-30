/*
 *  firebase_handler.cpp
 *
 *  firebase handler c
 *  Created on: 2023. 4. 3
 */

#include "firebase-handler.h"

#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"

FirebaseModule::FirebaseModule(uint8_t coreIndex) {
        fbdo = new FirebaseData;
        auth = new FirebaseAuth;
        config = new FirebaseConfig;
        serverTask = new TaskHandle_t;
        configTime(GMT_OFFSET_WIB, DAYLIGHT_OFFSET, NTP_SERVER);
        xTaskCreatePinnedToCore(serverHandler, "server_task", 20000, NULL, 1, serverTask, coreIndex);
}

FirebaseModule::~FirebaseModule() {
}

bool FirebaseModule::init() {
        (*config).api_key = API_KEY;
        (*config).database_url = DATABASE_URL;
        (*auth).user.email = USER_EMAIL;
        (*auth).user.password = USER_PASSWORD;

        (*config).token_status_callback = tokenStatusCallback;
        Firebase.begin(config, auth);
        Firebase.reconnectWiFi(true);
        Firebase.setDoubleDigits(2);  // default 5
        Firebase.setFloatDigits(2);
        (*config).timeout.serverResponse = 10 * 10000;
        return true;
}

bool FirebaseModule::connectToWiFi(const char* ssid, const char* pwd) {
        WiFi.mode(WIFI_OFF);
        delay(1000);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, pwd);
        Serial.print("Connecting to : ");
        Serial.println(ssid);
        while (WiFi.status() != WL_CONNECTED) {
                Serial.print(".");
                delay(1000);
        }
        Serial.println(WiFi.localIP());
        connect = true;
        return true;
}

bool FirebaseModule::isConnect() {
        return connect && Firebase.ready();
}

bool FirebaseModule::update(void (*onUpdate)(void)) {
        onUpdate();
        return true;
}

void FirebaseModule::clearData() {
        free(data);
        dataLen = 0;
        data = nullptr;
        for (int i = 0; i < addrLen; i++) {
                free(address[i]);
        }
        free(address);
        address = NULL;
        addrLen = 0;
}

void FirebaseModule::addData(float newData, const char* newAddressData) {
        dataLen++;
        data = (float*)realloc(data, dataLen * sizeof(float));
        data[dataLen - 1] = newData;

        char** newAddress = (char**)realloc(address, (addrLen + 1) * sizeof(char*));
        if (newAddress == NULL) return;
        address = newAddress;
        address[addrLen] = (char*)malloc(strlen(newAddressData) + 1);
        if (address[addrLen] == NULL) return;
        strcpy(address[addrLen], newAddressData);
        addrLen++;
}

void FirebaseModule::sendData(uint32_t __time) {
        try {
                if (data == nullptr) return;
                if (Firebase.ready() && millis() - sendTime >= __time) {
                        for (uint8_t i = 0; i < dataLen; i++) {
                                Firebase.RTDB.setFloatAsync(fbdo, address[i], data[i]) ? 1 : 0;
                        }
                        sendTime = millis();
                }
        } catch (...) {
        }
}

void FirebaseModule::setFloat(float floatData, const char* addrs) {
        try {
                if (Firebase.ready()) {
                        Firebase.RTDB.setFloatAsync(fbdo, addrs, floatData) ? 1 : 0;
                }
        } catch (...) {
        }
}

void FirebaseModule::setString(String strData, const char* addrs) {
        try {
                if (Firebase.ready()) {
                        Firebase.RTDB.setStringAsync(fbdo, addrs, strData) ? 1 : 0;
                }
        } catch (...) {
        }
}

float FirebaseModule::getData(const char* getAddress) {
        try {
                if (Firebase.ready() && Firebase.RTDB.getFloat(fbdo, F(getAddress))) {
                        return fbdo->to<float>();
                }
        } catch (...) {
        }
        return 0.0;
}

String FirebaseModule::getStrData(const char* getAddress) {
        try {
                if (Firebase.ready() && Firebase.RTDB.getString(fbdo, F(getAddress))) {
                        return fbdo->to<String>();
                }
        } catch (...) {
        }
        return "";
}

void FirebaseModule::waitConnection(uint32_t __tmr) {
        delay(__tmr);
}