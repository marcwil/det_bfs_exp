#!/bin/bash

download() {
    name=$1
    zip_url=$(curl -s http://networkrepository.com/$name | grep -o "http://nrvis.com/.*zip")
    wget $zip_url
}

download "TSOPF-FS-b162-c3"
download "kim1"
download "socfb-Oklahoma97"
