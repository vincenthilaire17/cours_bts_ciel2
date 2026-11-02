// Configuration UART 1 sur Raspberry Pi Pico 2 W
#define LoRaSerial Serial1

// Identifiants TTN (Format MSB standard)
const char* devEui = "70B3D57ED0079BCA";
const char* appEui = "0000000000000000";
const char* appKey = "5811A330CBC5737640BDF67FF1551B28";

const char* mot = "test";
unsigned long lastSendTime = 0;
const unsigned long interval = 15000; // Envoi toutes les 15s

// Fonction d'envoi de commande AT avec lecture complète de la réponse
String sendATCommand(String cmd, unsigned int timeout = 2000) {
  String response = "";
  
  // Vider le buffer de réception préalable
  while (LoRaSerial.available()) LoRaSerial.read();

  // Envoi de la commande terminée par \r\n
  LoRaSerial.print(cmd + "\r\n");
  Serial.print("-> ");
  Serial.println(cmd);

  unsigned long startTime = millis();
  while (millis() - startTime < timeout) {
    while (LoRaSerial.available()) {
      char c = LoRaSerial.read();
      response += c;
    }
  }
  
  Serial.print("<- ");
  if (response.length() > 0) {
    Serial.print(response);
  } else {
    Serial.println("[PAS DE REPONSE]");
  }
  return response;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // Broches UART1 matérielles du Pico 2 W
  LoRaSerial.setTX(0); // GP0 -> RX du LoRa-E5
  LoRaSerial.setRX(1); // GP1 -> TX du LoRa-E5
  
  // Débit standard LoRa-E5 (Basculer à 115200 si [PAS DE REPONSE])
  LoRaSerial.begin(9600); 

  Serial.println("==========================================");
  Serial.println("  Initialisation LoRa-E5 (Pico 2 W)       ");
  Serial.println("==========================================");
  delay(1000);

  // 1. Test basique de présence
  sendATCommand("AT", 1000);

  // 2. Mode LoRaWAN OTAA et Région Europe (EU868)
  sendATCommand("AT+MODE=LWOTAA", 1000);
  sendATCommand("AT+DR=EU868", 1000);

  // 3. Configuration des identifiants TTN
  sendATCommand("AT+ID=DevEui,\"" + String(devEui) + "\"", 1000);
  sendATCommand("AT+ID=AppEui,\"" + String(appEui) + "\"", 1000);
  sendATCommand("AT+KEY=APPKEY,\"" + String(appKey) + "\"", 1000);

  // 4. Demande de connexion au réseau TTN (Timeout élargi à 10s)
  Serial.println("\n--- Lancement du Join TTN ---");
  sendATCommand("AT+JOIN", 10000);
}

void loop() {
  if (millis() - lastSendTime >= interval) {
    lastSendTime = millis();

    // Envoi du message texte vers TTN
    Serial.println("\n--- Envoi d'une trame Uplink ---");
    sendATCommand("AT+MSG=\"" + String(mot) + "\"", 5000);
  }
}