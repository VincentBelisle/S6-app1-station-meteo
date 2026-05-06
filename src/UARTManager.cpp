#include "UARTManager.h"

UARTManager::UARTManager(Stream& stream)
    : stream_(&stream), rxBuffer_() {}

void UARTManager::begin() {
  rxBuffer_.reserve(64);
  stream_->println("UARTManager ready. Send GET_DATA to retrieve latest sensor values.");
}

void UARTManager::update() {
  while (stream_->available() > 0) {
    const char c = static_cast<char>(stream_->read());
    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      if (rxBuffer_.length() > 0) {
        handleCommand(rxBuffer_);
        rxBuffer_ = "";
      }
    } else {
      rxBuffer_ += c;
    }
  }
}

void UARTManager::setSensorData(const SensorData& data) {
  latestData_ = data;
}

void UARTManager::sendCurrentData() {
  stream_->print("DATA,");
  stream_->print("SEQ=");
  stream_->print(latestData_.sequence);
  stream_->print(",TEMP=");
  stream_->print(latestData_.temperatureC, 2);
  stream_->print(",HUM=");
  stream_->print(latestData_.humidityPercent, 2);
  stream_->print(",PRES=");
  stream_->print(latestData_.pressurePa);
  stream_->print(",WIND=");
  stream_->print(latestData_.windSpeedMps, 2);
  stream_->print(",WINDDIR=");
  stream_->print(latestData_.windDirectionDeg, 2);
  stream_->print(",LUX=");
  stream_->println(latestData_.illuminanceLux, 2);
}

void UARTManager::handleCommand(const String& command) {
  const String normalized = command;
  if (normalized.equalsIgnoreCase("GET_DATA") || normalized.equalsIgnoreCase("REQ_DATA")) {
    sendCurrentData();
    return;
  }

  stream_->print("ERR,UNKNOWN_CMD,");
  stream_->println(command);
}
