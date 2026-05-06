#include <Arduino.h>

#include "BleServerManager.h"

BleServerManager bleServer;

void setup()
{
  Serial.begin(115200);

  bleServer.begin();
}

void loop()
{
  bleServer.update();

  static unsigned long lastSensorMs = 0;
  const unsigned long sensorIntervalMs = 2000;
  unsigned long now = millis();
  if (now - lastSensorMs >= sensorIntervalMs) {
    lastSensorMs = now;

    // Notify clients that new sensor data is available
    // (All sensor data will be transmitted via UART on request)
    bleServer.notifyDataReady();
  }
}