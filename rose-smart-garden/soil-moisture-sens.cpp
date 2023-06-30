/*
 *  soil-moisture-sens.cpp
 *
 *  soil-moisture sensor c
 *  Created on: 2023. 4. 3
 */

#include "soil-moisture-sens.h"
#include "Arduino.h"
#include "sensor-filter.h"

#define SENSOR_FILTER_KF 8

SoilMoistureSens::SoilMoistureSens()
  : sensorPin(A0), isCalibrate(false) {
}

SoilMoistureSens::SoilMoistureSens(uint8_t __pin, bool enableCalibrate) {
        this->sensorPin = __pin;
        isCalibrate = enableCalibrate;
}

SoilMoistureSens::~SoilMoistureSens() {
        delete this;
}

void SoilMoistureSens::init() {
        pinMode(sensorPin, INPUT);
}

void SoilMoistureSens::update() {
        if (millis() - sensTimer[0] >= 500) {
                if (!isCalibrate) {
                        soilMoistureValue = analogRead(sensorPin);
                        soilMoistureValue = map(soilMoistureValue, 1023, 465, 0, 100);
                        soilMoistureValue = soilMoistureValue + (soilMoistureValue * SENSOR_FILTER_KF);
                        soilMoistureValue /= SENSOR_FILTER_KF + 1;
                } else {
                        KalmanFilter* sensorKf = new KalmanFilter(2, 2, 0.01);
                        arrSoilMoistureValue[SENS_RET_RAW_DATA] = analogRead(sensorPin);
                        arrSoilMoistureValue[SENS_RET_RAW_DATA] = map(arrSoilMoistureValue[SENS_RET_RAW_DATA], 1023, 465, 0, 100);
                        arrSoilMoistureValue[SENS_RET_ACT_DATA] = arrSoilMoistureValue[SENS_RET_RAW_DATA] + (arrSoilMoistureValue[SENS_RET_RAW_DATA] * SENSOR_FILTER_KF);
                        arrSoilMoistureValue[SENS_RET_ACT_DATA] /= SENSOR_FILTER_KF + 1;

                        arrSoilMoistureValue[SENS_RET_AVG_DATA] = getSensorAverage(arrSoilMoistureValue[SENS_RET_ACT_DATA]);
                        // arrSoilMoistureValue[SENS_RET_FILTERED_DATA] = lowPassFilter(arrSoilMoistureValue[SENS_RET_ACT_DATA], arrSoilMoistureValue[SENS_RET_FILTERED_DATA], 0.1);
                        arrSoilMoistureValue[SENS_RET_FILTERED_DATA] = sensorKf->updateEstimate(arrSoilMoistureValue[SENS_RET_ACT_DATA]);
                        delete sensorKf;
                }
                sensTimer[0] = millis();
        }
}

void SoilMoistureSens::debug() {
        if (millis() - sensTimer[1] >= 500) {
                if (isCalibrate) return;
                Serial.print("| soilMoistureValueRaw: ");
                Serial.print(soilMoistureValue);
                Serial.println();
                sensTimer[1] = millis();
        }
}

void SoilMoistureSens::calibrate() {
        if (millis() - sensTimer[2] >= 500) {
                if (!isCalibrate) return;
                Serial.print("| arrSoilMoistureValueRaw: ");
                Serial.print(arrSoilMoistureValue[SENS_RET_RAW_DATA]);
                Serial.print("| arrSoilMoistureValueAct: ");
                Serial.print(arrSoilMoistureValue[SENS_RET_ACT_DATA]);
                Serial.print("| arrSoilMoistureValueAvg: ");
                Serial.print(arrSoilMoistureValue[SENS_RET_AVG_DATA]);
                Serial.print("| arrSoilMoistureValueFiltered: ");
                Serial.print(arrSoilMoistureValue[SENS_RET_FILTERED_DATA]);
                Serial.println();
                sensTimer[2] = millis();
        }
}

void SoilMoistureSens::getValue(float* output) {
        *output = soilMoistureValue;
}

void SoilMoistureSens::getValue(int* output) {
}

void SoilMoistureSens::getValue(char* output) {
}

void SoilMoistureSens::setCallBack(void (*callbackFunc)(void)) {
        thisCallbackFunc = callbackFunc;
}

void SoilMoistureSens::count() {
}

void SoilMoistureSens::reset() {
}

float SoilMoistureSens::getValue(sens_ret_index_t c) {
        if (!isCalibrate) return soilMoistureValue;
        return arrSoilMoistureValue[c];
}

void SoilMoistureSens::setPins(uint8_t __pin) {
        this->sensorPin = __pin;
}

float SoilMoistureSens::getSensorAverage(float sensorValue, int numReadings) {
        float sensorSum = 0;
        for (int i = 0; i < numReadings; i++)
                sensorSum += sensorValue;
        return sensorSum / (float)numReadings;
}

float SoilMoistureSens::lowPassFilter(float input, float output, float alpha) {
        output = (alpha * input) + ((1.0 - alpha) * output);
        return output;
}