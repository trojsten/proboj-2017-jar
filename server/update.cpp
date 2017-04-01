#include "update.h"
#include <iostream>
#include <algorithm>

using namespace std;


// int own_territory(game_state& gs, int player) {
//     int points = 0;
//     return points;
// }

game_state update_game_state(game_state gs, vector<instruction> commands) {
    game_state new_gs = gs;
    
    new_gs.round++;
    sort(commands.begin(), commands.end());
    return new_gs;
}
