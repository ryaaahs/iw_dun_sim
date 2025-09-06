#!/usr/bin/env bash

make clean
make

FOLDER="iw_dun_sim_linux_build"

mkdir -p "$FOLDER"

cp "build/iw_dun_sim" "$FOLDER/"
cp "json/simulation_values_template.json" "$FOLDER/"

zip -r "${FOLDER}.zip" "$FOLDER"

if [ -d "$FOLDER" ]; then
    echo "Deleting folder $FOLDER..."
    rm -rf "$FOLDER"
fi
