#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Service UUID détecté lors de votre scan
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

// Caractéristique UUID standard associée à ce service ESP32 GATT
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

struct ServeurBLE {
  const char* nomCible;
  bool trouve;
  BLEAddress* pAddress;
};

// Uniquement les 2 serveurs demandés : B3 puis B2
ServeurBLE serveurs[] = {
  { "ESP32_BLE_GATT_B3", false, nullptr },
  { "ESP32_BLE_GATT_B2", false, nullptr }
};

const int NB_SERVEURS = 2;
int indexActuel = 0;

String donneeNotifiee = "";
bool notificationRecue = false;

// Callback si B2 ou B3 envoie des données par Notification
static void notifyCallback(BLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify) {
  for (size_t i = 0; i < length; i++) {
    char c = (char)pData[i];
    if (c != '\r' && c != '\n') {
      donneeNotifiee += c;
    }
  }
  if (donneeNotifiee.length() > 0) {
    notificationRecue = true;
  }
}

// Callback de scan : filtre strictement sur le Service UUID et les noms B2 / B3
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // 1. Vérifie si l'appareil possède le bon Service UUID
    if (!advertisedDevice.haveServiceUUID() || !advertisedDevice.isAdvertisingService(serviceUUID)) {
      return;
    }

    // 2. Vérifie si le nom correspond à ESP32_BLE_GATT_B3 ou ESP32_BLE_GATT_B2
    if (advertisedDevice.haveName()) {
      String nomDetecte = advertisedDevice.getName().c_str();
      
      for (int i = 0; i < NB_SERVEURS; i++) {
        if (nomDetecte == String(serveurs[i].nomCible)) {
          serveurs[i].trouve = true;
          if (serveurs[i].pAddress != nullptr) delete serveurs[i].pAddress;
          serveurs[i].pAddress = new BLEAddress(advertisedDevice.getAddress());

          Serial.print("[DÉTECTÉ] ");
          Serial.print(serveurs[i].nomCible);
          Serial.print(" | MAC : ");
          Serial.print(advertisedDevice.getAddress().toString().c_str());
          Serial.print(" | RSSI : ");
          Serial.print(advertisedDevice.getRSSI());
          Serial.println(" dBm");
        }
      }
    }
  }
};

void scannerServeursCibles() {
  Serial.println("\nRecherche de ESP32_BLE_GATT_B3 et ESP32_BLE_GATT_B2...");
  for (int i = 0; i < NB_SERVEURS; i++) {
    serveurs[i].trouve = false;
  }

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  pBLEScan->start(5, false);
  pBLEScan->clearResults();
}

void lireServeur(ServeurBLE &srv) {
  BLEClient* pClient = BLEDevice::createClient();

  if (!pClient->connect(*srv.pAddress)) {
    Serial.print("[ERREUR] Échec de connexion à ");
    Serial.println(srv.nomCible);
    delete pClient;
    return;
  }

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    pClient->disconnect();
    delete pClient;
    return;
  }

  // Récupère la caractéristique cible (ou la première disponible dans le service)
  BLERemoteCharacteristic* pRemoteChar = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteChar == nullptr) {
    auto* pChars = pRemoteService->getCharacteristics();
    if (pChars != nullptr && !pChars->empty()) {
      pRemoteChar = pChars->begin()->second;
    }
  }

  if (pRemoteChar != nullptr) {
    donneeNotifiee = "";
    notificationRecue = false;

    // 1. Abonnement aux notifications si supportées
    if (pRemoteChar->canNotify()) {
      pRemoteChar->registerForNotify(notifyCallback);
      
      if (pRemoteChar->canWrite() || pRemoteChar->canWriteNoResponse()) {
        pRemoteChar->writeValue((uint8_t*)"?", 1, false);
      }

      unsigned long debut = millis();
      while (millis() - debut < 800 && !notificationRecue) {
        delay(10);
      }
    }

    // 2. Affichage de la donnée (notification ou lecture directe)
    if (notificationRecue) {
      Serial.print(srv.nomCible);
      Serial.print(" -> ");
      Serial.println(donneeNotifiee);
    } else if (pRemoteChar->canRead()) {
      String valeurLue = pRemoteChar->readValue().c_str();
      valeurLue.trim();
      Serial.print(srv.nomCible);
      Serial.print(" -> ");
      Serial.println(valeurLue);
    }
  }

  pClient->disconnect();
  delete pClient;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  BLEDevice::init("ESP32_BLE_Client");
  scannerServeursCibles();
}

void loop() {
  ServeurBLE &cible = serveurs[indexActuel];

  if (cible.trouve && cible.pAddress != nullptr) {
    lireServeur(cible);
    delay(100);
  }

  indexActuel = (indexActuel + 1) % NB_SERVEURS;
}