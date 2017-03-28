#ifndef UPDATE_H
#define UPDATE_H

#include "common.h"
#include "mapa.h"

game_state update_game_state(game_state gs, vector<vector<player_command>> commands);

#endif
