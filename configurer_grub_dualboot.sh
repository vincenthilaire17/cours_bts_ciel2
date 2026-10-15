#!/bin/bash
# ==============================================================================
# BTS CIEL - Script de configuration automatique de GRUB pour Dual-Boot
# Fonctions :
#  1. Installe et exécute os-prober (détection de Windows 11).
#  2. Règle le délai d'attente à 10 secondes (GRUB_TIMEOUT=10).
#  3. Force l'affichage visible du menu interactif (GRUB_TIMEOUT_STYLE=menu).
#  4. Active os-prober dans GRUB (GRUB_DISABLE_OS_PROBER=false).
#  5. Propose de choisir le système par défaut (Windows 11 ou Ubuntu).
#  6. Exécute sudo update-grub.
# ==============================================================================

if [ "$EUID" -ne 0 ]; then
  echo -e "\033[1;31m[-] Erreur : Ce script doit être exécuté avec les privilèges root (sudo).\033[0m"
  echo "Usage : sudo bash $0"
  exit 1
fi

echo -e "\033[1;36m==========================================================================\033[0m"
echo -e "\033[1;36m       BTS CIEL - CONFIGURATION AUTOMATIQUE DE GRUB (DUAL-BOOT)          \033[0m"
echo -e "\033[1;36m==========================================================================\033[0m"
echo ""

# 1. Sauvegarde préventive
cp /etc/default/grub /etc/default/grub.backup.$(date +%Y%m%d_%H%M%S)
echo -e "\033[1;32m[OK]\033[0m Sauvegarde créée : /etc/default/grub.backup.*"

# 2. Installation os-prober
echo -e "\033[1;33m[*] Vérification de l'outil os-prober...\033[0m"
apt update -qq && apt install -y -qq os-prober > /dev/null 2>&1

# 3. Test de détection de Windows
WIN_ENTRY=$(os-prober | grep -i "windows")
if [ -n "$WIN_ENTRY" ]; then
  echo -e "\033[1;32m[OK]\033[0m Windows 11 détecté : $WIN_ENTRY"
else
  echo -e "\033[1;33m[!] Attention : Windows Boot Manager n'a pas été détecté automatiquement par os-prober.\033[0m"
fi

# 4. Choix de l'OS par défaut
echo ""
echo "Quel système d'exploitation souhaitez-vous démarrer PAR DÉFAUT après 10 secondes ?"
echo "  1) Windows 11 (Windows Boot Manager)"
echo "  2) Ubuntu (Linux)"
echo "  3) Mémoriser le dernier OS démarré (saved)"
read -p "Votre choix [1, 2 ou 3] (défaut: 1) : " CHOIX_OS

case "$CHOIX_OS" in
  2)
    GRUB_DEF_VAL="0"
    echo -e "\033[1;32m-> Choix validé : Ubuntu par défaut.\033[0m"
    ;;
  3)
    GRUB_DEF_VAL="saved"
    echo -e "\033[1;32m-> Choix validé : Dernier OS démarré mémorisé.\033[0m"
    ;;
  *)
    GRUB_DEF_VAL="\"Windows Boot Manager\""
    echo -e "\033[1;32m-> Choix validé : Windows 11 par défaut.\033[0m"
    ;;
esac

# 5. Modification de /etc/default/grub
sed -i '/^GRUB_TIMEOUT=/d' /etc/default/grub
sed -i '/^GRUB_TIMEOUT_STYLE=/d' /etc/default/grub
sed -i '/^GRUB_DEFAULT=/d' /etc/default/grub
sed -i '/^GRUB_SAVEDEFAULT=/d' /etc/default/grub
sed -i '/^GRUB_DISABLE_OS_PROBER=/d' /etc/default/grub

cat <<EOT >> /etc/default/grub

# Configuration Dual-Boot BTS CIEL
GRUB_DEFAULT=$GRUB_DEF_VAL
GRUB_TIMEOUT=10
GRUB_TIMEOUT_STYLE=menu
GRUB_DISABLE_OS_PROBER=false
EOT

if [ "$CHOIX_OS" == "3" ]; then
  echo "GRUB_SAVEDEFAULT=true" >> /etc/default/grub
fi

echo -e "\033[1;32m[OK]\033[0m Paramètres appliqués dans /etc/default/grub :"
echo "     - GRUB_TIMEOUT = 10 secondes"
echo "     - GRUB_TIMEOUT_STYLE = menu (visible)"
echo "     - GRUB_DEFAULT = $GRUB_DEF_VAL"
echo "     - GRUB_DISABLE_OS_PROBER = false"

# 6. Mise à jour de GRUB
echo ""
echo -e "\033[1;33m[*] Régénération du menu de démarrage (update-grub)...\033[0m"
update-grub

echo ""
echo -e "\033[1;32m==========================================================================\033[0m"
echo -e "\033[1;32m           CONFIGURATION TERMINÉE AVEC SUCCÈS !                          \033[0m"
echo -e "\033[1;32m==========================================================================\033[0m"
echo "Vous pouvez redémarrer avec : sudo reboot"
