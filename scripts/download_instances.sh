#!/bin/bash
if [ ! -d "instances" ]; then
    echo "Directory `instances` does not exist. Please call this script from the root of the project!"
    exit 1
fi

cd instances
echo "Downloading real.zip ..."
wget https://external-validity.iti.kit.edu/download/graphs-archives/real.zip
echo "Unzipping..."
unzip real.zip
