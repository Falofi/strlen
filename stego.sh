#!/bin/sh
# Compile ft_strlen_stego.c et recover.c dans build/, puis lance le bon.
#
#   ./stego.sh "chaîne"    cache la longueur de "chaîne" dans le fond d'écran
#   ./stego.sh --recover   affiche la longueur cachée dans le fond d'écran
#   ./stego.sh --restore   remet le fond d'écran d'origine

set -e
cd "$(dirname "$0")"

if [ $# -ne 1 ]; then
	echo "usage: $0 \"chaîne\" | --recover | --restore" >&2
	exit 1
fi

mkdir -p build
cc -Wall -Wextra ft_strlen_stego.c -ljpeg -lpng -o build/strlen_stego
cc -Wall -Wextra recover.c -ljpeg -lpng -o build/recover

case "$1" in
	--recover)
		exec build/recover ;;
	--restore)
		exec build/strlen_stego --restore ;;
	*)
		exec build/strlen_stego "$1" ;;
esac
