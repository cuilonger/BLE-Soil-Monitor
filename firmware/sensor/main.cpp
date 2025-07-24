#include <NimBLEDevice.h>

// Configuration
#define SENSOR_ID "SoilSensor1"  // Change for each sensor
const int MIN_RAW_VALUE = 800;
const int MAX_RAW_VALUE = 2500;
const int BROADCAST_INTERVAL = 2000;
const int VALUE_CHANGE_INTERVAL = 5000;

NimBLEAdvertising* pAdvertising;
int currentValue = 1500;
unsigned long lastBroadcastTime;
unsigned long lastValueChangeTime;

void updateSensorValue() {
  int change = random(-50, 51);
  currentValue = constrain(currentValue + change, MIN_RAW_VALUE, MAX_RAW_VALUE);
  
  Serial.print("New value: ");
  Serial.print(currentValue);
  Serial.print(" (");
  Serial.print(map(currentValue, MIN_RAW_VALUE, MAX_RAW_VALUE, 0, 100));
  Serial.println("%)");
}

void broadcastData() {
  uint8_t nameLen = strlen(SENSOR_ID);
  uint8_t mfgData[3 + nameLen];
  
  mfgData[0] = nameLen;
  memcpy(&mfgData[1], SENSOR_ID, nameLen);
  mfgData[1 + nameLen] = (currentValue >> 8) & 0xFF;
  mfgData[2 + nameLen] = currentValue & 0xFF;
  
  NimBLEAdvertisementData advertisementData;
  advertisementData.setFlags(0x06);
  advertisementData.setManufacturerData(std::string((char*)mfgData, sizeof(mfgData)));
  pAdvertising->setAdvertisementData(advertisementData);
  pAdvertising->start();
}

void setup() {
  Serial.begin(115200);
  NimBLEDevice::init(SENSOR_ID);
  pAdvertising = NimBLEDevice::getAdvertising();
  
  currentValue = random(MIN_RAW_VALUE, MAX_RAW_VALUE);
  lastValueChangeTime = millis();
  lastBroadcastTime = millis();
  
  Serial.print(SENSOR_ID);
  Serial.println(" initialized");
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastValueChangeTime >= VALUE_CHANGE_INTERVAL) {
    updateSensorValue();
    lastValueChangeTime = currentTime;
  }
  
  if (currentTime - lastBroadcastTime >= BROADCAST_INTERVAL) {
    broadcastData();
    lastBroadcastTime = currentTime;
  }
  
  delay(100);
}
