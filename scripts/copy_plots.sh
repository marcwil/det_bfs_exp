#!/bin/bash
if [ ! -d "../latex/plots/" ]; then
    echo "Directory `../latex/plots` does not exist."
    exit 1
fi

cp evaluation/output.pdf/* ../latex/plots/