#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <map>

#include "marshal.h"

using namespace std;

const int MAX_ROUNDS = 1000;

enum direction {
    LEFT, RIGHT, UP, DOWN
};

enum comand {
    MOVE, BUILD
};

struct instruction {
    int player;
    comand cmd;
    int id;
    direction dir;
    int strenght;
};

struct point {
    int x, y;
    
    bool operator== (const point A) const {
        return x == A.x && y == A.y;
    }
};

enum square_type {
    GRASS, STONE, WATER, LAB, LAB_SPAWN, TOWN
};

struct block {
    square_type type;
    int owned_by;
    int strenght;
};

struct player {
    point position;
    direction dir;
    bool alive;

    int score;
    int turbo;
};

// UCASTNICI: tento struct je pre vas nedolezity
struct mapa {
    int width, height;
    vector<vector<square_type>> squares;
    
    game_map(){}

    game_map(int width, int height);
    
    bool load (string filename) ;
};

struct game_state {
    int round;

    int width, height;

    vector<player> players;
    vector<block> blocks;

    game_state() {}
    game_state(int num_players, game_map gm);

    int block_index(point pos) {
        return pos.x * height + pos.y;
    }

    block get_block(point pos) {
        return blocks[block_index(pos)];
    }

    block get_block(int x, int y) {
        return get_block({x, y});
    }
};



#endif


#ifdef reflection
// tieto udaje pouziva marshal.cpp aby vedel ako tie struktury ukladat a nacitavat

reflection(point)
    member(x)
    member(y)
end()

reflection(block)
    member(type)
    member(owned_by)
    member(crossed_by)
end()

reflection(player)
    member(position)
    member(dir)
    member(alive)
    member(score)
end()

reflection(game_state)
    member(round)
    member(width)
    member(height)
    member(players)
    member(blocks)
end()

#endif
