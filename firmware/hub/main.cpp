#include <NimBLEDevice.h>
#include <map>

// BLE Configuration
const int SCAN_WINDOW = 2000;
const int REPORT_INTERVAL = 1000;

#define HUB_NAME "SoilMonitorHub"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define COMMAND_UUID        "beb5483e-36e1-4688-b7f5-ea07361b26a8"

struct SensorReading {
  String name;
  int rawValue;
  int moisturePct;
  unsigned long lastSeen;
};

std::map<String, SensorReading> sensorReadings;
NimBLECharacteristic* pCommandChar;
bool systemActive = true;

void sendSensorData();

class ScanCallbacks : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice* device) {
    if(!systemActive) return;
    
    if(device->haveManufacturerData()) {
      std::string data = device->getManufacturerData();
      if(data.length() >= 4) {
        uint8_t nameLen = data[0];
        if(data.length() == 1 + nameLen + 2) {
          String sensorName(data.substr(1, nameLen).c_str());
          int rawValue = (data[nameLen+1] << 8) | data[nameLen+2];
          rawValue = constrain(rawValue, 800, 2500);
          
          sensorReadings[sensorName] = {
            sensorName,
            rawValue,
            map(rawValue, 800, 2500, 0, 100),
            millis()
          };
        }
      }
    }
  }
};

class ServerCallbacks: public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer) {
    Serial.println("Client connected");
  }

  void onDisconnect(NimBLEServer* pServer) {
    Serial.println("Client disconnected");
    NimBLEDevice::startAdvertising();
  }
};

class CommandCallbacks: public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      String command = String(value.c_str());
      
      if (command == "enable") {
        systemActive = true;
        Serial.println("System enabled");
      } 
      else if (command == "disable") {
        systemActive = false;
        Serial.println("System disabled");
      }
      else if (command == "status") {
        sendSensorData();
      }
    }
  }
};

void sendSensorData() {
  if (!pCommandChar) return;

  String dataStr;
  for (const auto& reading : sensorReadings) {
    const SensorReading& r = reading.second;
    dataStr += r.name + "," + String(r.rawValue) + "," + String(r.moisturePct) + ";";
  }
  
  pCommandChar->setValue(dataStr);
  pCommandChar->notify();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Soil Monitor Hub...");

  NimBLEDevice::init(HUB_NAME);
  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  pCommandChar = pService->createCharacteristic(
    COMMAND_UUID,
    NIMBLE_PROPERTY::WRITE |
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::NOTIFY
  );
  pCommandChar->setCallbacks(new CommandCallbacks());
  pService->start();

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();

  NimBLEScan* scan = NimBLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new ScanCallbacks());
  scan->setActiveScan(true);
  scan->setInterval(100);
  scan->setWindow(99);
}

void loop() {
  if (!NimBLEDevice::getScan()->isScanning()) {
    NimBLEDevice::getScan()->start(SCAN_WINDOW/1000, false);
  }

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= REPORT_INTERVAL && systemActive) {
    sendSensorData();
    lastUpdate = millis();
  }

  // Clean up old readings (15s timeout)
  unsigned long threshold = millis() - 15000;
  for (auto it = sensorReadings.begin(); it != sensorReadings.end(); ) {
    if (it->second.lastSeen < threshold) {
      it = sensorReadings.erase(it);
    } else {
      ++it;
    }
  }

  delay(10);
}
