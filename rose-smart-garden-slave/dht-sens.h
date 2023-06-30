/*
 *  dht-sens.h
 *
 *  dht sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef DHT_SENS_H
#define DHT_SENS_H

#include "Arduino.h"
#include "sensor-module.h"
#include "stdint.h"
#include "DHT.h"

class DHTSens : public BaseSens {
      private:
        DHT* sensorClass;
        float arrTemperatureValue[6];
        float arrHumidityValue[6];
        float temperatureValue;
        float humidityValue;
        uint32_t sensTimer[3];

        bool isCalibrate;
        uint8_t sensorPin;

        void (*thisCallbackFunc)(void) = nullptr;
        float getSensorAverage(float sensorValue, int numReadings = 10);
        float lowPassFilter(float input, float output, float alpha);
      public:
        DHTSens();
        DHTSens(uint8_t __pin, bool enableCalibrate = false);
        ~DHTSens();

        void init() override;
        void update() override;
        void debug() override;
        void calibrate() override;

        void getValue(float* output) override;
        void getValue(int* output) override;
        void getValue(char* output) override;

        void setCallBack(void (*callbackFunc)(void)) override;
        void count() override;
        void reset() override;

        float getValue(sens_ret_index_t c = SENS_RET_ACT_DATA);
        void setPins(uint8_t __pin);
};

#endif  // DHT_SENS_H