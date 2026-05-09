#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_DPS310.h>
#include <SparkFun_Weather_Meter_Kit_Arduino_Library.h>

#define DHTTYPE DHT11
#define DHTPIN 16
#define LIGHT_PIN 34

// SparkFun Weather Meter Kit pins on ESP32
int windDirectionPin = 35;
int windSpeedPin = 27;
int rainfallPin = 23;

SFEWeatherMeterKit weatherMeterKit(windDirectionPin, windSpeedPin, rainfallPin);

DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_DPS310 dps;
uint32_t delayMS;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  dht.begin();

  // Configure ADC for light sensor on GPIO34 (input-only pin)
  pinMode(LIGHT_PIN, INPUT);
  weatherMeterKit.setADCResolutionBits(12);
  weatherMeterKit.begin();

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = 1000;

  Serial.println("DPS310");
  if (!dps.begin_I2C())
  {
    Serial.println("Failed to find DPS");
    while (1)
      yield();
  }
  Serial.println("DPS OK!");

  // Setup highest precision
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  // Here we create a struct to hold all the calibration parameters
  SFEWeatherMeterKitCalibrationParams calibrationParams = weatherMeterKit.getCalibrationParams();

  // The wind vane has 8 switches, but 2 could close at the same time, which
  // results in 16 possible positions. Each position has a resistor connected
  // to GND, so this library assumes a voltage divider is created by adding
  // another resistor to VCC. Some of the wind vane resistor values are
  // fairly close to each other, meaning an accurate ADC is required. However
  // some ADCs have a non-linear behavior that causes this measurement to be
  // inaccurate. To account for this, the vane resistor values can be manually
  // changed here to compensate for the non-linear behavior of the ADC
  calibrationParams.vaneADCValues[WMK_ANGLE_0_0] = 1674;
  calibrationParams.vaneADCValues[WMK_ANGLE_22_5] = 1677;
  calibrationParams.vaneADCValues[WMK_ANGLE_45_0] = 208;
  calibrationParams.vaneADCValues[WMK_ANGLE_67_5] = 108;
  calibrationParams.vaneADCValues[WMK_ANGLE_90_0] = 571;
  calibrationParams.vaneADCValues[WMK_ANGLE_112_5] = 570;
  calibrationParams.vaneADCValues[WMK_ANGLE_135_0] = 337;
  calibrationParams.vaneADCValues[WMK_ANGLE_157_5] = 981;
  calibrationParams.vaneADCValues[WMK_ANGLE_180_0] = 2342;
  calibrationParams.vaneADCValues[WMK_ANGLE_202_5] = 2341;
  calibrationParams.vaneADCValues[WMK_ANGLE_225_0] = 3855;
  calibrationParams.vaneADCValues[WMK_ANGLE_247_5] = 3854;
  calibrationParams.vaneADCValues[WMK_ANGLE_270_0] = 3472;
  calibrationParams.vaneADCValues[WMK_ANGLE_292_5] = 3470;
  calibrationParams.vaneADCValues[WMK_ANGLE_315_0] = 2973;
  calibrationParams.vaneADCValues[WMK_ANGLE_337_5] = 2974;

  // Now we can set all the calibration parameters at once
  weatherMeterKit.setCalibrationParams(calibrationParams);
}

void loop()
{

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println("Error reading temperature!");
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
  }

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println("Error reading humidity!");
  }
  else
  {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println(" %");
  }

  // DPS310 barometre: lire les deux mesures sur le meme echantillon.
  sensors_event_t temp_event, pressure_event;

  if (dps.temperatureAvailable() || dps.pressureAvailable())
  {
    dps.getEvents(&temp_event, &pressure_event);

    Serial.print(F("Temperature = "));
    Serial.print(temp_event.temperature);
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(pressure_event.pressure);
    Serial.println(" hPa");

    Serial.println();
  }
  else
  {
    Serial.println("DPS310 data not ready");
  }

  // Lire capteur de lumière Grove (branché sur GPIO34)
  int lightRaw = analogRead(LIGHT_PIN);
  Serial.print("Light raw: ");
  Serial.print(lightRaw);

  Serial.print(F("Wind direction (degrees): "));
  Serial.print(weatherMeterKit.getWindDirection(), 1);

  Serial.print(F("\t\t"));
  Serial.print(F("Wind speed (kph): "));
  Serial.print(weatherMeterKit.getWindSpeed(), 1);
  Serial.print(F("\t\t"));
  Serial.print(F("Total rainfall (mm): "));
  Serial.println(weatherMeterKit.getTotalRainfall(), 1);

  delay(delayMS);
}
