#!/bin/sh
make

if [ "$(id -u)" -ne 0 ]; then
    echo "The script needs to be run as root, however you can use doas or sudo or whatever you want" >&2
    exit 1
fi

chmod +x ccover
chmod +x src/lyrics.sh

cp -r ccover /usr/local/bin/
cp -r src/lyrics.sh /usr/local/bin/

echo "¡CCover installed!"
echo "Run ccover to see the cover"
echo "Run lyrics.sh to see the lyrics"
