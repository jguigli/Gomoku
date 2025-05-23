#!/bin/bash

echo "🔍 Détection du système..."

OS="$(uname)"
echo "Système détecté : $OS"

echo "$OS"

if [[ "$OS" == "Linux" ]]; then
    echo "🟢 Linux détecté"
    if ! command -v apt &> /dev/null; then
        echo "❌ 'apt' n'est pas disponible. Ce script ne supporte actuellement que les systèmes basés sur Debian/Ubuntu."
        exit 1
    fi

    echo "🔄 Mise à jour des paquets..."
    sudo apt update

    echo "📦 Installation de raylib..."
    sudo apt install -y libraylib-dev

    echo "✅ Raylib installé avec apt (Linux)"
    
elif [[ "$OS" == "Darwin" ]]; then
    echo "🍎 macOS détecté"

    if ! command -v brew &> /dev/null; then
        echo "❌ Homebrew non installé. Installe-le d'abord : https://brew.sh"
        exit 1
    fi

    echo "📦 Installation de raylib avec Homebrew..."
    brew install raylib

    echo "✅ Raylib installé avec Homebrew (macOS)"

else
    echo "❌ OS non supporté automatiquement par ce script ($OS)"
    exit 1
fi
