/*
Auteurs: Vincent Bélisle BELV1805,
Elliot Gaulin GAUE1909
*/
#pragma once

#include <Arduino.h>

class UARTManager {
 public:
  struct SensorData {
    float temperatureC = 0.0f;
    float humidityPercent = 0.0f;
    float pressurehPa = 0;
    float windSpeedMps = 0.0f;
    float windDirectionDeg = 0.0f;
    float illuminanceLux = 0.0f;
    float totalRainfallMm = 0.0f;
    uint32_t sequence = 0;
  };

  explicit UARTManager(Stream& stream = Serial);

  void begin();
  void update();
  void setSensorData(const SensorData& data);
  void sendCurrentData();

 private:
  void handleCommand(const String& command);

  Stream* stream_;
  SensorData latestData_;
  String rxBuffer_;
};
