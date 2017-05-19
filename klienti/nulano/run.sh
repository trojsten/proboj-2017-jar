#!/bin/bash

mkdir -p ../../zaznamy/$2
../../server/server ../../zaznamy/$2 $1 ../nulano/ ../nulano/ ../hlupy/ 2> ../../zaznamy/$2/server.log
