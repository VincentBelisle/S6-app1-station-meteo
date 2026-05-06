#pragma once

#include <Arduino.h>
#include <BLECharacteristic.h>
#include <BLEServer.h>

class BleServerManager {
 public:
  BleServerManager();
  void begin();
  void update();
  // Notify clients that new sensor data is available (they will request via UART)
  void notifyDataReady();

 private:
  class ServerCallbacks;

  void onConnect();
  void onDisconnect();

  BLEServer* server_;
  BLECharacteristic* dataReadyCharacteristic_;
  uint32_t dataReadyCounter_;
  bool deviceConnected_;
  bool oldDeviceConnected_;
};
