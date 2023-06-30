/*
 *  dht-sens.cpp
 *
 *  dht sensor c
 *  Created on: 2023. 4. 3
 */

#include "dht-sens.h"
#include "Arduino.h"
#include "sensor-filter.h"

#define SENSOR_FILTER_KF 8

DHTSens::DHTSens()
  : sensorPin(A0), isCalibrate(false) {
}

DHTSens::DHTSens(uint8_t __pin, bool enableCalibrate) {
        this->sensorPin = __pin;
        isCalibrate = enableCalibrate;
}

DHTSens::~DHTSens() {
        delete this;
}

void DHTSens::init() {
        sensorClass = new DHT(sensorPin, DHT22);
        sensorClass->begin();
}

void DHTSens::update() {
        if (millis() - sensTimer[0] >= 500) {
                if (!isCalibrate) {
                        temperatureValue = sensorClass->readTemperature();
                        temperatureValue = temperatureValue + (temperatureValue * SENSOR_FILTER_KF);
                        temperatureValue /= SENSOR_FILTER_KF + 1;

                        humidityValue = sensorClass->readHumidity();
                        humidityValue = humidityValue + (humidityValue * SENSOR_FILTER_KF);
                        humidityValue /= SENSOR_FILTER_KF + 1;
                } else {
                        KalmanFilter* sensorKf = new KalmanFilter(2, 2, 0.01);
                        arrTemperatureValue[SENS_RET_RAW_DATA] = sensorClass->readTemperature();
                        arrTemperatureValue[SENS_RET_ACT_DATA] = arrTemperatureValue[SENS_RET_RAW_DATA] + (arrTemperatureValue[SENS_RET_RAW_DATA] * SENSOR_FILTER_KF);
                        arrTemperatureValue[SENS_RET_ACT_DATA] /= SENSOR_FILTER_KF + 1;

                        arrTemperatureValue[SENS_RET_AVG_DATA] = getSensorAverage(arrTemperatureValue[SENS_RET_ACT_DATA]);
                        arrTemperatureValue[SENS_RET_FILTERED_DATA] = sensorKf->updateEstimate(arrTemperatureValue[SENS_RET_ACT_DATA]);

                        arrHumidityValue[SENS_RET_RAW_DATA] = sensorClass->readHumidity();
                        arrHumidityValue[SENS_RET_ACT_DATA] = arrHumidityValue[SENS_RET_RAW_DATA] + (arrHumidityValue[SENS_RET_RAW_DATA] * SENSOR_FILTER_KF);
                        arrHumidityValue[SENS_RET_ACT_DATA] /= SENSOR_FILTER_KF + 1;

                        arrHumidityValue[SENS_RET_AVG_DATA] = getSensorAverage(arrHumidityValue[SENS_RET_ACT_DATA]);
                        arrHumidityValue[SENS_RET_FILTERED_DATA] = sensorKf->updateEstimate(arrHumidityValue[SENS_RET_ACT_DATA]);
                        delete sensorKf;
                }
                sensTimer[0] = millis();
        }
        delete sensorClass;
        sensorClass = nullptr;
}

void DHTSens::debug() {
        if (millis() - sensTimer[1] >= 500) {
                if (isCalibrate) return;
                Serial.print("| temperatureValueRaw: ");
                Serial.print(temperatureValue);

                Serial.print("| humidityValueRaw: ");
                Serial.print(humidityValue);
                Serial.println();
                sensTimer[1] = millis();
        }
}

void DHTSens::calibrate() {
        if (millis() - sensTimer[2] >= 500) {
                if (!isCalibrate) return;
                Serial.print("| arrTemperatureValueRaw: ");
                Serial.print(arrTemperatureValue[SENS_RET_RAW_DATA]);
                Serial.print("| arrTemperatureValueAct: ");
                Serial.print(arrTemperatureValue[SENS_RET_ACT_DATA]);
                Serial.print("| arrTemperatureValueAvg: ");
                Serial.print(arrTemperatureValue[SENS_RET_AVG_DATA]);
                Serial.print("| arrTemperatureValueFiltered: ");
                Serial.print(arrTemperatureValue[SENS_RET_FILTERED_DATA]);

                Serial.print("| arrHumidityValueRaw: ");
                Serial.print(arrHumidityValue[SENS_RET_RAW_DATA]);
                Serial.print("| arrHumidityValueAct: ");
                Serial.print(arrHumidityValue[SENS_RET_ACT_DATA]);
                Serial.print("| arrHumidityValueAvg: ");
                Serial.print(arrHumidityValue[SENS_RET_AVG_DATA]);
                Serial.print("| arrHumidityValueFiltered: ");
                Serial.print(arrHumidityValue[SENS_RET_FILTERED_DATA]);
                Serial.println();
                sensTimer[2] = millis();
        }
}

void DHTSens::getValue(float* output) {
        output[0] = temperatureValue;
        output[1] = humidityValue;
}

void DHTSens::getValue(int* output) {
}

void DHTSens::getValue(char* output) {
}

void DHTSens::setCallBack(void (*callbackFunc)(void)) {
        thisCallbackFunc = callbackFunc;
}

void DHTSens::count() {
}

void DHTSens::reset() {
}

float DHTSens::getValue(sens_ret_index_t c) {
        if (!isCalibrate) return temperatureValue;
        return arrTemperatureValue[c];
}

void DHTSens::setPins(uint8_t __pin) {
        this->sensorPin = __pin;
}

float DHTSens::getSensorAverage(float sensorValue, int numReadings) {
        float sensorSum = 0;
        for (int i = 0; i < numReadings; i++)
                sensorSum += sensorValue;
        return sensorSum / (float)numReadings;
}

float DHTSens::lowPassFilter(float input, float output, float alpha) {
        output = (alpha * input) + ((1.0 - alpha) * output);
        return output;
}