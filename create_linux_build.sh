#!/usr/bin/env bash

make clean
make

FOLDER="iw_dun_sim_linux_build"
TAR_FILE="iw_dun_sim_linux_build.tar.gz"
JSON_FOLDER="json"

mkdir -p "$FOLDER"
mkdir -p "$FOLDER/$JSON"

cp "build/iw_dun_sim" "$FOLDER/"
cp "json/simulation_values_template.json" "$FOLDER/$JSON"

if [ -f "$TAR_FILE" ]; then
    echo "Deleting file $TAR_FILE..."
    rm -f "$TAR_FILE"
fi

tar czf "${FOLDER}.tar.gz" "$FOLDER"

if [ -d "$FOLDER" ]; then
    echo "Deleting folder $FOLDER..."
    rm -rf "$FOLDER"
fi
