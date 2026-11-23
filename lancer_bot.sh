#!/usr/bin/env bash
# Lance le bot Discord strlen.
# Usage : ./lancer_bot.sh
# Ordre de recherche du token :
#   1. la variable DISCORD_BOT_TOKEN, si elle est définie ;
#   2. le fichier ~/.config/strlen-bot/token (ou $STRLEN_TOKEN_FILE), hors du projet ;
#   3. sinon, une demande en saisie masquée.
# Le token n'est jamais écrit dans le dossier du projet.
set -euo pipefail

cd "$(dirname "$0")"

TOKEN_FILE="${STRLEN_TOKEN_FILE:-$HOME/.config/strlen-bot/token}"

if [ -z "${DISCORD_BOT_TOKEN:-}" ] && [ -f "$TOKEN_FILE" ]; then
    DISCORD_BOT_TOKEN="$(tr -d '[:space:]' < "$TOKEN_FILE")"
    export DISCORD_BOT_TOKEN
fi

if [ -z "${DISCORD_BOT_TOKEN:-}" ]; then
    printf 'Token du bot (saisie masquée) : '
    read -rs DISCORD_BOT_TOKEN
    echo
    export DISCORD_BOT_TOKEN
fi

if [ ! -f libstrlen.so ] || [ libstrlen.c -nt libstrlen.so ]; then
    echo "Compilation de libstrlen.so..."
    cc -shared -fPIC -Wall -Wextra -Werror libstrlen.c -o libstrlen.so
fi

if ! python3 -c "import discord" 2>/dev/null; then
    echo "discord.py n'est pas installé : pip install discord.py" >&2
    exit 1
fi

exec python3 bot.py
