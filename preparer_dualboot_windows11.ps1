<#
.SYNOPSIS
    Script d'automatisation de la préparation au Dual-Boot Windows 11 & Ubuntu (BTS CIEL).
.DESCRIPTION
    1. Élévation automatique des privilèges Administrateur (UAC).
    2. Désactivation du Démarrage Rapide (Fast Startup) pour éviter les corruptions NTFS sous Linux.
    3. Vérification du chiffrement BitLocker / Chiffrement de l'appareil :
       - Sauvegarde de la clé de récupération 48 chiffres dans un fichier texte.
       - Suspension temporaire de la protection pour 2 redémarrages (évite le blocage TPM).
    4. Réduction automatique du volume C: de 100 Go (102 400 Mo) en espace libre non alloué.
.AUTHOR
    Vincent Hilaire - BTS CIEL
#>

[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$Host.UI.RawUI.WindowTitle = "BTS CIEL - Préparation Automatique au Dual-Boot (100 Go)"

# -----------------------------------------------------------------------------
# 1. Vérification et auto-élévation des privilèges Administrateur
# -----------------------------------------------------------------------------
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    Write-Host "[!] Demande d'élévation en mode Administrateur..." -ForegroundColor Yellow
    try {
        Start-Process powershell.exe -ArgumentList ("-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`"") -Verb RunAs
    } catch {
        Write-Host "[-] Échec de l'élévation des privilèges. Vous devez exécuter ce script en tant qu'Administrateur." -ForegroundColor Red
    }
    exit
}

Clear-Host
Write-Host "==========================================================================" -ForegroundColor Cyan
Write-Host "       BTS CIEL - AUTOMATISATION PRÉ-DUAL BOOT WINDOWS 11 & UBUNTU        " -ForegroundColor Cyan
Write-Host "==========================================================================" -ForegroundColor Cyan
Write-Host ""

# -----------------------------------------------------------------------------
# 2. Désactivation du Démarrage Rapide (Fast Startup)
# -----------------------------------------------------------------------------
Write-Host "[*] Étape 1/3 : Désactivation du Démarrage Rapide (Fast Startup)..." -ForegroundColor Yellow

try {
    # Modification du registre
    Set-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Power" -Name "HiberbootEnabled" -Value 0 -Type DWord -Force
    # Désactivation de l'hibernation pour libérer hiberfil.sys
    & powercfg.exe /h off
    Write-Host "    [OK] Démarrage Rapide et Hibernation désactivés avec succès." -ForegroundColor Green
} catch {
    Write-Host "    [!] Attention : Impossible de modifier les paramètres d'alimentation : $_" -ForegroundColor DarkYellow
}

Write-Host ""

# -----------------------------------------------------------------------------
# 3. Vérification et Sécurisation de BitLocker / Chiffrement de l'appareil
# -----------------------------------------------------------------------------
Write-Host "[*] Étape 2/3 : Vérification du chiffrement BitLocker / Chiffrement OEM..." -ForegroundColor Yellow

$desktopPath = [Environment]::GetFolderPath("Desktop")
$keyFile = Join-Path $desktopPath "CLE_RECUPERATION_BITLOCKER_$(env:COMPUTERNAME).txt"

try {
    $bitlocker = Get-BitLockerVolume -MountPoint "C:" -ErrorAction SilentlyContinue

    if ($bitlocker -and $bitlocker.ProtectionStatus -eq "On") {
        Write-Host "    [i] Chiffrement détecté actif sur le volume C:." -ForegroundColor Cyan
        
        # Récupération de la clé de secours
        $keyProtector = $bitlocker.KeyProtector | Where-Object { $_.KeyProtectorType -eq "RecoveryPassword" }
        if ($keyProtector) {
            $recoveryKey = $keyProtector.RecoveryPassword
            $keyContent = @"
===================================================================
BTS CIEL - CLÉ DE RÉCUPÉRATION BITLOCKER / CHIFFREMENT MACHINE
Machine : $env:COMPUTERNAME
Date    : $(Get-Date -Format 'dd/MM/yyyy HH:mm:ss')
Volume  : C:
-------------------------------------------------------------------
CLÉ NUMÉRIQUE DE RÉCUPÉRATION (48 CHIFFRES) :
$recoveryKey
===================================================================
NOTE : Si Windows réclame cette clé après installation d'Ubuntu,
saisissez les 48 chiffres ci-dessus à l'écran de déverrouillage.
"@
            Set-Content -Path $keyFile -Value $keyContent -Encoding UTF8 -Force
            Write-Host "    [OK] Clé de récupération (48 chiffres) sauvegardée sur le Bureau :" -ForegroundColor Green
            Write-Host "         $keyFile" -ForegroundColor White
            Write-Host "         CLÉ : $recoveryKey" -ForegroundColor Magenta
        }

        # Suspension temporaire de la protection pour 2 redémarrages
        Write-Host "    [*] Suspension temporaire de la protection pour 2 redémarrages..." -ForegroundColor Yellow
        Suspend-BitLocker -MountPoint "C:" -RebootCount 2 -ErrorAction SilentlyContinue
        Write-Host "    [OK] BitLocker temporairement suspendu (pas de blocage TPM au prochain boot)." -ForegroundColor Green
    } else {
        Write-Host "    [OK] Aucun chiffrement actif sur C: (BitLocker désactivé)." -ForegroundColor Green
    }
} catch {
    Write-Host "    [!] Vérification BitLocker ignorée ou non applicable : $_" -ForegroundColor DarkGray
}

Write-Host ""

# -----------------------------------------------------------------------------
# 4. Partitionnement : Réduction de 100 Go sur le volume C:
# -----------------------------------------------------------------------------
Write-Host "[*] Étape 3/3 : Réduction du volume C: de 100 Go (102 400 Mo)..." -ForegroundColor Yellow

$tailleAReduireMo = 102400
$tailleAReduireOctets = [int64]$tailleAReduireMo * 1024 * 1024

try {
    $partition = Get-Partition -DriveLetter "C" -ErrorAction Stop
    $supportedSize = Get-PartitionSupportedSize -DriveLetter "C" -ErrorAction Stop
    
    $tailleActuelleGo = [math]::Round($partition.Size / 1GB, 2)
    $tailleMinGo = [math]::Round($supportedSize.SizeMin / 1GB, 2)
    $espaceLibreGo = [math]::Round(($partition.Size - $supportedSize.SizeMin) / 1GB, 2)
    $nouvelleTailleOctets = $partition.Size - $tailleAReduireOctets
    $nouvelleTailleGo = [math]::Round($nouvelleTailleOctets / 1GB, 2)

    Write-Host "    - Taille actuelle du volume C: : $tailleActuelleGo Go" -ForegroundColor White
    Write-Host "    - Espace réductible disponible  : $espaceLibreGo Go (Minimum supporté : $tailleMinGo Go)" -ForegroundColor White

    if ($nouvelleTailleOctets -lt $supportedSize.SizeMin) {
        Write-Host ""
        Write-Host "[-] ERREUR : Espace disque insuffisant pour libérer 100 Go !" -ForegroundColor Red
        Write-Host "    Espace réductible : $espaceLibreGo Go disponible(s)." -ForegroundColor Yellow
        Write-Host "    Veuillez faire de la place sur votre disque C: (vider la corbeille, supprimer les fichiers volumineux)." -ForegroundColor Yellow
        Write-Host ""
        Read-Host "Appuyez sur Entrée pour quitter"
        exit
    }

    Write-Host "    [*] Réduction de la partition C: en cours..." -ForegroundColor Cyan
    Resize-Partition -DriveLetter "C" -Size $nouvelleTailleOctets -ErrorAction Stop
    
    Write-Host "    [OK] Réduction validée ! Nouvelle taille de C: : $nouvelleTailleGo Go." -ForegroundColor Green
    Write-Host "    [OK] Espace non alloué créé : 100 Go (102 400 Mo) prêt pour Ubuntu !" -ForegroundColor Green

} catch {
    Write-Host "[-] Échec lors du redimensionnement de la partition : $_" -ForegroundColor Red
    Write-Host "    Si des fichiers système non déplaçables bloquent la réduction :" -ForegroundColor Yellow
    Write-Host "    1. Redémarrez une première fois votre PC." -ForegroundColor Yellow
    Write-Host "    2. Relancez ce script." -ForegroundColor Yellow
    Read-Host "Appuyez sur Entrée pour quitter"
    exit
}

Write-Host ""
Write-Host "==========================================================================" -ForegroundColor Green
Write-Host "          OPÉRATION TERMINÉE AVEC SUCCÈS SUR CETTE MACHINE !              " -ForegroundColor Green
Write-Host "==========================================================================" -ForegroundColor Green
Write-Host ""
Write-Host "RÉCAPITULATIF POUR L'ATELIER BTS CIEL :" -ForegroundColor White
Write-Host " 1. Le Démarrage Rapide Windows est DÉSACTIVÉ." -ForegroundColor White
Write-Host " 2. Le volume C: a été réduit de 100 Go (zone non allouée créée)." -ForegroundColor White
if (Test-Path $keyFile) {
    Write-Host " 3. Clé BitLocker sauvegardée sur le Bureau : $keyFile" -ForegroundColor Yellow
}
Write-Host ""
Write-Host "PROCHAINE ÉTAPE :" -ForegroundColor Cyan
Write-Host "  -> Branchez la clé USB d'installation Ubuntu." -ForegroundColor White
Write-Host "  -> Redémarrez le PC en tapotant la touche de Boot Menu (F12 / F9 / F8)." -ForegroundColor White
Write-Host "  -> Sélectionnez 'UEFI: [Nom de la clé]' pour installer Ubuntu." -ForegroundColor White
Write-Host ""

Read-Host "Appuyez sur Entrée pour fermer cette fenêtre..."
