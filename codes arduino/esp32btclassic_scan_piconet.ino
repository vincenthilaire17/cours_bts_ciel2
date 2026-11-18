#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

struct BTDevice {
  const char* nom;
  uint8_t adresse[6];
  bool trouveAuScan;
};

// Vos 3 modules esclaves dans l'ordre souhaité : BT3 -> BT2 -> BT1
// Remplacez ces adresses MAC par celles relevées lors du scan ou via les commandes AT
BTDevice appareils[] = {
  { "BT3", {0x00, 0x0E, 0xEA, 0xCF, 0x86, 0xF9}, false },
  { "BT2", {0x00, 0x0E, 0xEA, 0xCF, 0x87, 0x0D}, false },
  { "BT1", {0x00, 0x0E, 0xEA, 0xCF, 0x86, 0xF2}, false }
};

const int NOMBRE_APPAREILS = 3;
int indexActuel = 0;

// Timeout d'écoute maximal sur chaque module (en ms) si aucune trame n'est reçue
const unsigned long DUREE_ECOUTE_MAX = 5000; 

// Variables pour le chronométrage du cycle de 3 relevés
unsigned long debutCycle = 0;
int nbRelevesDansCycle = 0;

// Fonction utilitaire pour comparer deux adresses MAC de 6 octets
bool memeAdresse(const uint8_t* addr1, const uint8_t* addr2) {
  for (int i = 0; i < 6; i++) {
    if (addr1[i] != addr2[i]) return false;
  }
  return true;
}

// Fonction de scan Bluetooth Classique (Inquiry)
void scannerAppareils() {
  Serial.println("\n=======================================================");
  Serial.println("  Démarrage du scan Bluetooth Classic (10 secondes)...");
  Serial.println("=======================================================");
  
  // Réinitialise l'état de détection
  for (int i = 0; i < NOMBRE_APPAREILS; i++) {
    appareils[i].trouveAuScan = false;
  }

  // Scan synchrone pendant 10000 ms (10 secondes)
  BTScanResults* resultats = SerialBT.discover(10000);

  if (resultats) {
    int nbTrouves = resultats->getCount();
    Serial.print("Scan terminé. Périphériques Bluetooth détectés : ");
    Serial.println(nbTrouves);

    for (int i = 0; i < nbTrouves; i++) {
      BTAdvertisedDevice* dev = resultats->getDevice(i);
      const uint8_t* macDetectee = *dev->getAddress().getNative();

      // Vérifie si l'adresse MAC correspond à BT1, BT2 ou BT3
      for (int j = 0; j < NOMBRE_APPAREILS; j++) {
        if (memeAdresse(macDetectee, appareils[j].adresse)) {
          appareils[j].trouveAuScan = true;
          Serial.print("  [OK] ");
          Serial.print(appareils[j].nom);
          Serial.print(" trouvé (Nom annoncé : ");
          Serial.print(dev->getName().c_str());
          Serial.print(", RSSI : ");
          Serial.print(dev->getRSSI());
          Serial.println(" dBm)");
        }
      }
    }
  } else {
    Serial.println("Erreur lors du lancement du scan.");
  }

  // Bilan des modules manquants
  for (int j = 0; j < NOMBRE_APPAREILS; j++) {
    if (!appareils[j].trouveAuScan) {
      Serial.print("  [ABSENT] ");
      Serial.print(appareils[j].nom);
      Serial.println(" n'a pas été détecté lors du scan.");
    }
  }
  Serial.println("=======================================================\n");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Démarrage de l'ESP32 en mode Maître (true = master)
  if (!SerialBT.begin("ESP32_Master", true)) {
    Serial.println("Erreur critique d'initialisation du Bluetooth !");
    while (1);
  }

  // Configuration du code PIN commun aux 3 modules esclaves
  SerialBT.setPin("1234", 4);
  Serial.println("Code PIN '1234' configuré.");

  // Lancement du scan initial
  scannerAppareils();
}

void loop() {
  // Début du chronométrage au lancement du 1er équipement du cycle
  if (indexActuel == 0) {
    debutCycle = millis();
    nbRelevesDansCycle = 0;
    Serial.println("-------------------------------------------------------");
    Serial.println(">>> DÉBUT D'UN NOUVEAU CYCLE DE 3 RELEVÉS PICONET <<<");
    Serial.println("-------------------------------------------------------");
  }

  BTDevice cible = appareils[indexActuel];

  // Si le module n'a pas été vu au scan, on l'ignore pour ne pas bloquer la boucle
  if (!cible.trouveAuScan) {
    Serial.print("Ignoré (absent au scan) : ");
    Serial.println(cible.nom);
  } else {
    Serial.print("--- Tentative de connexion à ");
    Serial.print(cible.nom);
    Serial.println(" ---");

    unsigned long tDebutConnexion = millis();
    bool connecte = SerialBT.connect(cible.adresse);

    if (connecte) {
      unsigned long tConnecte = millis();
      Serial.print("  [CONNECTÉ] à ");
      Serial.print(cible.nom);
      Serial.printf(" (temps de liaison : %lu ms)\n", tConnecte - tDebutConnexion);

      unsigned long debutEcoute = millis();
      bool trameRecue = false;

      // Écoute de la trame émise par le Nano esclave (ex: "Bonjour BTx")
      while ((millis() - debutEcoute < DUREE_ECOUTE_MAX) && !trameRecue) {
        if (SerialBT.available()) {
          String texteRecu = SerialBT.readStringUntil('\n');
          texteRecu.trim();
          
          if (texteRecu.length() > 0) {
            Serial.print("  >>> Donnée reçue [");
            Serial.print(cible.nom);
            Serial.print("] : \"");
            Serial.print(texteRecu);
            Serial.println("\"");
            trameRecue = true;
            nbRelevesDansCycle++;
          }
        }
        delay(10);
      }

      if (!trameRecue) {
        Serial.print("  [TIMEOUT] Aucune donnée reçue de ");
        Serial.println(cible.nom);
      }

      // Déconnexion immédiate pour libérer le canal radio et passer à l'esclave suivant
      SerialBT.disconnect();
      Serial.print("  [DÉCONNECTÉ] de ");
      Serial.println(cible.nom);

    } else {
      Serial.print("  [ÉCHEC] Connexion impossible à ");
      Serial.println(cible.nom);
    }
  }

  // Si le dernier module du cycle vient d'être traité, on calcule et affiche le temps global
  if (indexActuel == NOMBRE_APPAREILS - 1) {
    unsigned long dureeTotaleCycle = millis() - debutCycle;
    Serial.println("\n=======================================================");
    Serial.println("       BILAN CHRONOMÉTRIQUE DU CYCLE (3 RELEVÉS)       ");
    Serial.println("=======================================================");
    Serial.printf("Temps total mesuré pour 3 relevés : %lu ms (soit %.2f s)\n", 
                  dureeTotaleCycle, dureeTotaleCycle / 1000.0);
    Serial.printf("Nombre de relevés réussis          : %d / %d\n", 
                  nbRelevesDansCycle, NOMBRE_APPAREILS);
    if (nbRelevesDansCycle > 0) {
      Serial.printf("Temps moyen par esclave interrogé : %lu ms\n", 
                    dureeTotaleCycle / NOMBRE_APPAREILS);
    }
    Serial.println("=======================================================\n");
  }

  // Passage au module suivant (BT3 -> BT2 -> BT1 -> BT3...)
  indexActuel = (indexActuel + 1) % NOMBRE_APPAREILS;
  delay(1000); // Temps de garde inter-cycles
}
