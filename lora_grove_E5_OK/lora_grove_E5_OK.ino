#include <BLEDevice.h>
#include <BLEUtils.h>

BLEAdvertising *pAdvertising;
int compteur = 0;

void setup() {
  Serial.begin(115200);
  
  // 1. Initialiser le BLE (Attention : max 15 caractères pour le nom sur ESP32 classique)
  BLEDevice::init("ESP32_KEYES");

  // 2. Obtenir l'objet d'advertising
  pAdvertising = BLEDevice::getAdvertising();
}

void loop() {
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

  // Nom diffusé dans la trame
  oAdvertisementData.setName("ESP32_KEYES");

  // Construction des données du fabricant (Manufacturer Data)
  String mfgData = "";
  mfgData += (char)0xFF; // ID Fabricant (octet 1)
  mfgData += (char)0xFF; // ID Fabricant (octet 2)
  mfgData += (char)(compteur & 0xFF);         // Octet de poids faible
  mfgData += (char)((compteur >> 8) & 0xFF);  // Octet de poids fort

  oAdvertisementData.setManufacturerData(mfgData);

  // Charger et lancer la diffusion
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->start();

  Serial.print("Trame envoyee depuis Keyestudio : ");
  Serial.println(compteur);

  compteur++;
  delay(1000);

  pAdvertising->stop();
}