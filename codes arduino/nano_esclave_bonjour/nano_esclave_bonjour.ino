/* =========================================================================
   BTS CIEL - Nœud Esclave Bluetooth Classic (SPP) pour Topologie Piconet
   Cible : Arduino Nano v3 + Platine Carrier ASX00061
   Module sans fil : Grove - Serial Bluetooth v3.0 / HC-05 (Port J7 : A0/A1)
   ========================================================================= */

#include <SoftwareSerial.h>

// Déclaration de la liaison série logicielle sur le connecteur Grove J7
// Broche A1 = RX (reçoit du TX du module Bluetooth via le fil blanc)
// Broche A0 = TX (émet vers le RX du module Bluetooth via le fil jaune)
SoftwareSerial BT_Serial(A1, A0);

// Identifiant du module à personnaliser pour chaque poste : "BT1", "BT2" ou "BT3"
const char* NOM_MODULE = "BT1";

unsigned long dernierEnvoi = 0;
const unsigned long PERIODE_ENVOI_MS = 1000; // Cadence d'émission : 1 seconde

void setup() {
  // Console série USB pour le contrôle local
  Serial.begin(115200);
  while (!Serial && millis() < 2000);

  // Initialisation de la liaison série avec le module Bluetooth (9600 bauds d'usine)
  BT_Serial.begin(9600);

  Serial.println(F("=================================================="));
  Serial.print(F("  Nœud Esclave Piconet initialisé : "));
  Serial.println(NOM_MODULE);
  Serial.println(F("=================================================="));
  Serial.println(F("Attente de scrutation par la passerelle ESP32..."));
}

void loop() {
  unsigned long tempsActuel = millis();

  // Émission cadencée et non-bloquante du message vers le maître
  // Dès que l'ESP32 se connecte en RFCOMM, il reçoit immédiatement la ligne
  if (tempsActuel - dernierEnvoi >= PERIODE_ENVOI_MS) {
    dernierEnvoi = tempsActuel;

    // Transmission Bluetooth : "Bonjour <Nom>\n"
    BT_Serial.print(F("Bonjour "));
    BT_Serial.print(NOM_MODULE);
    BT_Serial.print(F("\n")); // Délimiteur de fin de ligne indispensable

    // Trace locale sur la console USB du PC
    Serial.print(F("[TX Bluetooth] Bonjour "));
    Serial.println(NOM_MODULE);
  }

  // Relais d'éventuels ordres reçus depuis le maître ESP32
  if (BT_Serial.available()) {
    char c = BT_Serial.read();
    Serial.write(c);
  }
}
