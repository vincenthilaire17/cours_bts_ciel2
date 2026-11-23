#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int dureeScan = 5; // Durée de chaque scan en secondes
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("MAC : ");
    Serial.print(advertisedDevice.getAddress().toString().c_str());

    Serial.print(" | RSSI : ");
    Serial.print(advertisedDevice.getRSSI());
    Serial.print(" dBm");

    Serial.print(" | Nom : ");
    if (advertisedDevice.haveName()) {
      Serial.print(advertisedDevice.getName().c_str());
    } else {
      Serial.print("(inconnu)");
    }

    if (advertisedDevice.haveServiceUUID()) {
      Serial.print(" | Service UUID : ");
      Serial.print(advertisedDevice.getServiceUUID().toString().c_str());
    }

    if (advertisedDevice.haveManufacturerData()) {
      Serial.print(" | Données : ");
      Serial.print(advertisedDevice.getManufacturerData().c_str());
    }

    Serial.println();
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation du scanner BLE...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Demande le nom complet et scan response
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}

void loop() {
  Serial.println("\n--- Nouveau scan BLE (5s) ---");
  BLEScanResults* foundDevices = pBLEScan->start(dureeScan, false);
  
  Serial.print("Total trouvé : ");
  Serial.println(foundDevices->getCount());
  
  pBLEScan->clearResults(); // Libère la mémoire avant le scan suivant
  delay(2000);
}
