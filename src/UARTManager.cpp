#include "UARTManager.h"

UARTManager::UARTManager(Stream &stream) : stream_(&stream) {}

void UARTManager::begin() {
  if (stream_) {
    // Print header once
    stream_->println("UARTManager ready");
  }
}

void UARTManager::update() {
  if (!stream_) return;
  while (stream_->available()) {
    int c = stream_->read();
    if (c == '\r') continue;
    if (c == '\n') {
      if (rxBuffer_.length()) {
        handleCommand(rxBuffer_);
        rxBuffer_.clear();
      }
    } else {
      rxBuffer_.concat((char)c);
    }
  }
}

void UARTManager::setSensorData(const SensorData &data) {
  latestData_ = data;
}

void UARTManager::sendCurrentData() {
  if (!stream_) return;
  // Simple CSV: seq,tempC,hum%,pres_hPa,wind_mps,wind_deg,light, rain_mm
  char buf[200];
  int n = snprintf(buf, sizeof(buf), "%lu,%.2f,%.2f,%.1f,%.2f,%.1f,%.0f,%.2f",
                   (unsigned long)latestData_.sequence,
                   latestData_.temperatureC,
                   latestData_.humidityPercent,
                   latestData_.pressurehPa,
                   latestData_.windSpeedMps,
                   latestData_.windDirectionDeg,
                   latestData_.illuminanceLux,
                   latestData_.totalRainfallMm);
  stream_->println(buf);
}

void UARTManager::handleCommand(const String &command) {
  if (command == "GET") {
    sendCurrentData();
  } else if (command == "RESET_RAIN") {
    // no-op here; application can implement if needed
    stream_->println("OK");
  } else {
    stream_->print("ERR Unknown cmd: ");
    stream_->println(command);
  }
}
