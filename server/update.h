#ifndef UPDATE_H
#define UPDATE_H

#include "common.h"
#include "mapa.h"

game_state update_game_state(mapa gm, game_state gs, vector<instruction> commands);

#endif
