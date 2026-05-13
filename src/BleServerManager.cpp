/*
Auteurs: Vincent Bélisle BELV1805,
Elliot Gaulin GAUE1909
*/

#include "BleServerManager.h"

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEUtils.h>

namespace {
constexpr uint16_t kEnvSensingServiceUuid = 0x181A;
constexpr uint16_t kDeviceInfoServiceUuid = 0x180A;

constexpr uint16_t kManufacturerNameCharacteristicUuid = 0x2A29;
constexpr uint16_t kModelNumberCharacteristicUuid = 0x2A24;
// Custom UUID for DataReady notification
constexpr const char* kDataReadyCharacteristicUuid = "FFFFFFFF-FFFF-FFFF-FFFF-000000000001";

}  // namespace

class BleServerManager::ServerCallbacks : public BLEServerCallbacks {
 public:
  explicit ServerCallbacks(BleServerManager& owner) : owner_(owner) {}

  void onConnect(BLEServer* /*server*/) override {
    owner_.onConnect();
  }

  void onDisconnect(BLEServer* /*server*/) override {
    owner_.onDisconnect();
  }

 private:
  BleServerManager& owner_;
};

BleServerManager::BleServerManager()
    : server_(nullptr),
      dataReadyCharacteristic_(nullptr),
      dataReadyCounter_(0),
      deviceConnected_(false),
      oldDeviceConnected_(false) {}

void BleServerManager::begin() {
  BLEDevice::init("Weather Station");

  server_ = BLEDevice::createServer();
  server_->setCallbacks(new ServerCallbacks(*this));

  BLEService* envService = server_->createService(BLEUUID(kEnvSensingServiceUuid));

  // DataReady characteristic: notifies clients when new sensor data is available
  const uint32_t notifyProps = BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY;
  dataReadyCharacteristic_ = envService->createCharacteristic(
      BLEUUID(kDataReadyCharacteristicUuid),
      notifyProps);
  dataReadyCharacteristic_->addDescriptor(new BLE2902());

  envService->start();

  BLEService* deviceInfoService = server_->createService(BLEUUID(kDeviceInfoServiceUuid));
  BLECharacteristic* manufacturerCharacteristic = deviceInfoService->createCharacteristic(
      BLEUUID(kManufacturerNameCharacteristicUuid),
      BLECharacteristic::PROPERTY_READ);
  manufacturerCharacteristic->setValue("DIY");

  BLECharacteristic* modelCharacteristic = deviceInfoService->createCharacteristic(
      BLEUUID(kModelNumberCharacteristicUuid),
      BLECharacteristic::PROPERTY_READ);
  modelCharacteristic->setValue("S6-app1");

  deviceInfoService->start();

  BLEAdvertising* advertising = server_->getAdvertising();
  advertising->addServiceUUID(BLEUUID(kEnvSensingServiceUuid));
  advertising->start();

  Serial.println("Advertising Environmental Sensing Service with DataReady");
}

void BleServerManager::update() {
  if (!deviceConnected_ && oldDeviceConnected_) {
    delay(200);
    server_->startAdvertising();
    Serial.println("Started advertising again...");
    oldDeviceConnected_ = false;
  }

  if (deviceConnected_ && !oldDeviceConnected_) {
    oldDeviceConnected_ = true;
  }
}

void BleServerManager::onConnect() {
  deviceConnected_ = true;
  Serial.println("Device connected");
}

void BleServerManager::onDisconnect() {
  deviceConnected_ = false;
  Serial.println("Device disconnected");
}

// Sensor sampling moved to application layer (main.cpp). DataReady notification is provided.

void BleServerManager::notifyDataReady() {
  // Increment counter and notify clients that new data is available
  dataReadyCounter_++;
  dataReadyCharacteristic_->setValue(reinterpret_cast<uint8_t*>(&dataReadyCounter_), sizeof(dataReadyCounter_));
  if (deviceConnected_) {
    dataReadyCharacteristic_->notify();
    Serial.print("Notified DataReady (counter: ");
    Serial.print(dataReadyCounter_);
    Serial.println(")");
  }
}
