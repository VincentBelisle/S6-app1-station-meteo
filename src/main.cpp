#include <Arduino.h>

#include "BleServerManager.h"
#include "UARTManager.h"

BleServerManager bleServer;
UARTManager uartManager;

void setup()
{
  Serial.begin(115200);

  bleServer.begin();
  uartManager.begin();
}

void loop()
{
  bleServer.update();
  uartManager.update();

  static unsigned long lastSensorMs = 0;
  const unsigned long sensorIntervalMs = 2000;
  unsigned long now = millis();
  if (now - lastSensorMs >= sensorIntervalMs) {
    lastSensorMs = now;

    UARTManager::SensorData data;
    data.sequence = now / sensorIntervalMs;
    data.temperatureC = 22.50f + static_cast<float>((now / 1000UL) % 15);
    data.humidityPercent = 45.00f + static_cast<float>((now / 1000UL) % 12);
    data.pressurePa = 101000UL + ((now / 1000UL) % 80UL);
    data.windSpeedMps = 3.00f + static_cast<float>((now / 1000UL) % 2);
    data.windDirectionDeg = static_cast<float>((now / 1000UL) * 37 % 360);
    data.illuminanceLux = 120.0f + static_cast<float>((now / 1000UL) % 400);

    uartManager.setSensorData(data);
    bleServer.notifyDataReady();
  }
}