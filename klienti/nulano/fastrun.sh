#!/bin/bash

#../../server/server ../../zaznamy/$1 ./mapy/kvadranty21x21-4.ppm . ../hlupy/ ../hlupy/ 2> ../../zaznamy/$1/server.log

#../../observer/observer ../../zaznamy/$1/observation

./run.sh ../../mapy/simple-medium60x60-4.ppm fast
./show.sh fast
