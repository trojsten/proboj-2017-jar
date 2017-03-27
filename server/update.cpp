#include "update.h"
#include <iostream>

using namespace std;

int kill_player(game_state& gs, int dier) {
    int blocks = 0;

    // delete all crossed blocks
    gs.players[dier].alive = false;
    for (unsigned i = 0; i < gs.blocks.size(); i++) {
    if (gs.blocks[i].crossed_by == dier) {
        gs.blocks[i].crossed_by = -1;
    }
    if (gs.blocks[i].owned_by == dier) {
        blocks++;
    }
    }

    return blocks;
}

int own_body(game_state& gs, int player) {
    int points = 0;

    // mark all crossed blocks as owned
    for (unsigned i = 0; i < gs.blocks.size(); i++) {
    if (gs.blocks[i].crossed_by == player) {
        if (gs.blocks[i].owned_by != player) points++;
        gs.blocks[i].owned_by = player;
        gs.blocks[i].crossed_by = -1;
    }
    }

    return points;
}

int own_territory(game_state& gs, int player) {
    int points = 0;

    points += own_body(gs, player);

    vector<bool> visited(gs.blocks.size(), false);
    for (int x = 0; x < gs.width; x++) {
    for (int y = 0; y < gs.height; y++) {
        int i = gs.block_index({x, y});

        if (visited[i]) continue;

        bool should_own = true;

        vector<pair<int, int>> stack;
        vector<bool> tested(gs.blocks.size(), false);
        stack.push_back({x, y});

        while (!stack.empty()) {
        int x = stack.back().first;
        int y = stack.back().second;
        int i = gs.block_index({x, y});
        stack.pop_back();

        if (gs.blocks[i].owned_by == player) continue;

        if (tested[i]) continue;
        tested[i] = true;
        
        if (x == 0 || y == 0 || x == gs.width - 1 || y == gs.height - 1) {
            should_own = false;
            break;
        }

        stack.push_back({x - 1, y});
        stack.push_back({x + 1, y});
        stack.push_back({x, y - 1});
        stack.push_back({x, y + 1});
        }

        if (should_own) {
        for (unsigned i = 0; i < gs.blocks.size(); i++) {
            if (tested[i]) {
            gs.blocks[i].owned_by = player;
            points++;
            }
        }
        }
    }
    }

    return points;
}

game_state update_game_state(game_state gs, vector<vector<player_command> > commands) {
    // pre kazdeho z hracov zistime jeho novy smer
    for (unsigned i = 0; i < gs.players.size(); i++) {
        if (!gs.players[i].alive) {
            continue;
        }
        if (!commands[i].empty()) {
            gs.players[i].dir = commands[i].back().dir;
        }
    }
    
    for (int turbo_step = 1; turbo_step >= 0; turbo_step--) {
        // turbo_step == 1: hybu sa len turbo hraci
        // turbo_step == 0: hybu sa vsetci hraci
        game_state new_gs = gs;
        
        // zistime novu poziciu kazdeho hraca
        for (unsigned i = 0; i < gs.players.size(); i++) {
            if (!gs.players[i].alive) continue;
            if (turbo_step && gs.players[i].turbo <= 0) continue;
        
            point new_position = new_gs.players[i].position;
            switch (new_gs.players[i].dir) {
            case LEFT:
                new_position.x -= 1;
                break;
            case RIGHT:
                new_position.x += 1;
                break;
            case UP:
                new_position.y -= 1;
                break;
            case DOWN:
                new_position.y += 1;
                break;
            }
            new_gs.players[i].position = new_position;
        }

        // zistime, ci sa nezrazili
        for (unsigned i = 0; i < new_gs.players.size(); i++) {
            if (!gs.players[i].alive) continue;
        if (turbo_step && new_gs.players[i].turbo <= 0) continue;

            // nezabil sa o stenu / okraj mapy?
            if (new_gs.players[i].position.x < 0 || new_gs.players[i].position.x >= new_gs.width) {
                kill_player(new_gs, i);
        cerr << "hrac " << i << " narazil do okraja mapy" << endl;
            } else if (new_gs.players[i].position.y < 0 || new_gs.players[i].position.y >= new_gs.height) {
                kill_player(new_gs, i);
        cerr << "hrac " << i << " narazil do okraja mapy" << endl;
            } else {
                // nezabili sme prave niekoho?
                block curr = gs.blocks[gs.block_index(new_gs.players[i].position)];
                if (curr.crossed_by != -1) {
                    if (curr.owned_by != curr.crossed_by) {
                        int blocks_killed = kill_player(new_gs, curr.crossed_by);
            cerr << "hrac " << curr.crossed_by << " bol zabity hracom " << i << endl;

            if (curr.crossed_by != i) {
                new_gs.players[i].score += 32 + blocks_killed / 4;
            }
                    }
                }
            }
        }
        
        // zistime, ci nenarazili do steny
        for (unsigned i = 0; i < new_gs.players.size(); i++) {
            if (!new_gs.players[i].alive) {
                continue;
            }
            if (new_gs.get_block(new_gs.players[i].position).type == WALL) {
                kill_player(new_gs, i);
        cerr << "hrac " << i << " narazil do steny" << endl;
            }
        }
        
        // zistime celne zrazky
        for (unsigned i = 0; i < new_gs.players.size(); i++) {
            if (!new_gs.players[i].alive) {
                continue;
            }
            for (unsigned j = 0; j < new_gs.players.size(); j++) {
                if (i == j) {
                    continue;
                }
                if (!gs.players[j].alive) {
                    continue;
                }
                if (new_gs.players[i].position == new_gs.players[j].position) {
                    new_gs.players[j].score += 32 + kill_player(new_gs, i) / 4;
                    new_gs.players[i].score += 32 + kill_player(new_gs, j) / 4;
            cerr << "hraci " << i << " a " << j << " sa celne zrazili" << endl;
                }
            }
        }
        
        // kazdy hrac obsadi svoje policko
        for (unsigned i = 0; i < new_gs.players.size(); i++) {
            if (!new_gs.players[i].alive) {
                continue;
            }
            int pos_index = new_gs.block_index(new_gs.players[i].position);
            new_gs.blocks[pos_index].crossed_by = i;
        }

        // zistime, ci hrac neobsadil uzemie
        for (unsigned i = 0; i < new_gs.players.size(); i++) {
            if (!new_gs.players[i].alive) {
                continue;
            }
            int pos_index = new_gs.block_index(new_gs.players[i].position);
            if (new_gs.blocks[pos_index].owned_by == (int)i) {
                new_gs.players[i].score += own_territory(new_gs, (int)i);
            }
        }
    
        // kazdy hrac zozerie bonus na svojom policku
        for (unsigned i = 0; i < new_gs.players.size(); i++) {
            if (!new_gs.players[i].alive) {
                continue;
            }
            int pos_index = new_gs.block_index(new_gs.players[i].position);
            if (new_gs.blocks[pos_index].type == FAST_BONUS) {
                new_gs.blocks[pos_index].type = BONUS_SPAWN;
                // ziska dvojnasobnu rychlost
                new_gs.players[i].turbo += TURBO_DURATION;
            }
            else
            if (new_gs.blocks[pos_index].type == STONE_BONUS) {
                new_gs.blocks[pos_index].type = BONUS_SPAWN;
                // svoje telo premeni na uzemie
                new_gs.players[i].score += own_body(new_gs, i);
            }
        }
    
        gs = new_gs;
    }
    
    // turbo hracom znizime turbo
    for (unsigned i = 0; i < gs.players.size(); i++) {
        if (!gs.players[i].alive) {
            continue;
        }
        if (gs.players[i].turbo > 0) {
            gs.players[i].turbo--;
        }
    }
    
    // vytvori na mape bonusy
    // na spawne sa moze objavit bonus len vtedy, ked nie je pokryty telom hadika
    for (unsigned i = 0; i < gs.blocks.size(); i++) {
        if (gs.blocks[i].type != BONUS_SPAWN) {
            continue;
        }
        if (gs.blocks[i].crossed_by != -1) {
            continue;
        }
        if (rand() % BONUS_PROBABILITY == 0) {
            // ktory bonus sa objavil?
            if (rand() % 2) {
                gs.blocks[i].type = FAST_BONUS;
            }
            else {
                gs.blocks[i].type = STONE_BONUS;
            }
        }
    }

    gs.round++;

    return gs;
}
