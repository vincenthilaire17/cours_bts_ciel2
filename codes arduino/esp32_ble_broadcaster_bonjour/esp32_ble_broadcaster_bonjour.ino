/* =========================================================================
   BTS CIEL - Émetteur BLE Broadcast (Broadcaster / Balise d'Annonce)
   Cible : ESP32 (Diffusion sans connexion sur canaux 37, 38 et 39)
   Diffuse le message : "Bonjour <Nom>" dans les paquets d'Advertising
   ========================================================================= */

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>

// Nom unique de la balise pour chaque binôme : "BLE1", "BLE2" ou "BLE3"
const char* NOM_BALISE = "BLE1"; 

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("==================================================");
  Serial.printf("  Démarrage Broadcaster BLE : %s\n", NOM_BALISE);
  Serial.println("==================================================");

  // 1. Initialisation du contrôleur BLE
  BLEDevice::init(NOM_BALISE);

  // 2. Configuration des données d'annonces (Advertising Data - max 31 octets)
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

  BLEAdvertisementData advData;
  advData.setFlags(0x06); // Mode LE General Discoverable + BR/EDR Not Supported
  advData.setName(NOM_BALISE);

  // Charge utile : message textuel "Bonjour <Nom>"
  String message = "Bonjour " + String(NOM_BALISE);
  advData.setManufacturerData(message.c_str());

  pAdvertising->setAdvertisementData(advData);

  // 3. Cadencement des annonces (intervalle de 100 ms : 160 * 0.625 ms = 0x00A0)
  pAdvertising->setMinInterval(0x00A0);
  pAdvertising->setMaxInterval(0x00A0);

  // 4. Lancement de la diffusion continue sans connexion
  pAdvertising->start();

  Serial.printf("[BROADCAST ACTIF] Message diffusé : \"%s\"\n", message.c_str());
  Serial.println("Diffusion omnidirectionnelle sur les canaux 37, 38, 39.");
}

void loop() {
  // L'émission des salves d'annonces est gérée par le contrôleur radio BLE matériel
  delay(2000);
}
