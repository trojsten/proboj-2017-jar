#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"


#define DEBUG

#ifdef DEBUG
    #define dbg true
#elif
    #define dbg false
#endif

mapa _m;
masked_game_state _stav;   // vzdy som hrac cislo 0

int dx[4] = {0,0,0,0};
int dy[4] = {0,0,0,0};

// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
    // (sem patri vas kod)

    // smery
    // vyska/sirka <-> riadok/stlpec
    tie(dx[DOLE], dy[DOLE])     = make_pair(+1, 0);
    tie(dx[VLAVO], dy[VLAVO])   = make_pair(0, -1);
    tie(dx[VPRAVO], dy[VPRAVO]) = make_pair(0, +1);
    tie(dx[HORE], dy[HORE])     = make_pair(-1, 0);
}

bool is_square_valid(const int x, const int y, const masked_game_state &state, const mapa &m) {
    // it is possible to step in here (at least theoretically)
    return (0 <= x && x < state.vyska && 0 <= y && y < state.sirka && m.squares[x][y] != KAMEN);
}

vector<pair<int,int> > get_my_robots(const masked_game_state &state) {
    vector<pair<int, int> > robots;
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (state.mapa[i][j].sila_robota > 0 && state.mapa[i][j].majitel == 0) {
                robots.push_back(make_pair(i, j));
            }
        }
    }
    return robots;
}

vector<pair<int, int> > get_my_labs(const masked_game_state &state, const mapa &m) {
    vector<pair<int, int> > labs;
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (state.mapa[i][j].majitel == 0 && m.squares[i][j] == LAB) {
                labs.push_back(make_pair(i, j));
            }
        }
    }
    return labs;
}

bool is_weaker_space(const int x, const int y, const int my_power, const masked_game_state &state, const mapa &m) {
    for (int i = 0; i < 4; i++) {
        const int nx = x + dx[i];
        const int ny = y + dy[i];
        if (is_square_valid(nx, ny, state, m) && state.mapa[nx][ny].majitel != 0 && state.mapa[nx][ny].sila_robota >= my_power) {
            return false;
        }
    }
    return true;
}

vector<Prikaz> get_commands(masked_game_state state, mapa m) {
    vector<Prikaz> instructions;

    vector<pair<int, int> > my_robots = get_my_robots(state);
    vector<bool> unmoved(my_robots.size(), true);
    if (dbg) {
        cerr << "MY ROBOTS:" << endl;
        for (int i = 0; i < my_robots.size(); i++) {
            cerr << "[" << my_robots[i].first << ", " << my_robots[i].second << "] ";
        }
        cerr << ";;" << endl;
    }

    vector<pair<int, int> > my_labs = get_my_labs(state, m);
    if (dbg) {
        cerr << "MY LABS:" << endl;
        for (int i = 0; i < my_labs.size(); i++) {
            cerr << "[" << my_labs[i].first << ", " << my_labs[i].second << "] ";
        }
        cerr << ";;" << endl;
    }

    // decide whether to build new robots (of course yes!)
    if (my_labs.size() > 0 && state.eter > 0) {
        random_shuffle(my_labs.begin(), my_labs.end());
        vector<bool> use_labs(my_labs.size(), false);
        for (int i = 0; i < my_labs.size(); i++) {
            const int lx = my_labs[0].first;
            const int ly = my_labs[0].second;
            bool successfully_moved = true;
            if (state.mapa[lx][ly].sila_robota != 0) {
                // treba posunut tohto robota
                successfully_moved = false;
                for (int k = 0; k < 4; k++) {
                    const int nx = lx + dx[k];
                    const int ny = ly + dy[k];
                    if (!is_square_valid(nx, ny, state, m)) continue;
                    if (state.mapa[nx][ny].sila_robota < state.mapa[lx][ly].sila_robota &&
                                state.mapa[nx][ny].majitel != 0 || state.mapa[nx][ny].sila_robota == 0) {
                        // posunieme sem!
                        Prikaz p;
                        p.pr = POSUN;
                        p.riadok = lx; p.stlpec = ly;
                        p.instrukcia = k;
                        instructions.push_back(p);

                        for (int j = 0; j < my_robots.size(); j++) if (my_robots[j] == make_pair(nx, ny)) {
                            unmoved[j] = false;
                        }
                        successfully_moved = true;
                        break;
                    }
                }
            }
            if (successfully_moved) {
                use_labs[i] = true;
            }
        }

        int usable_labs_count = 0;
        for (int i = 0; i < use_labs.size(); i++) {
            if (use_labs[i]) {
                usable_labs_count += 1;
            }
        }

        int cc = 0;
        for (int t = 0; t < my_labs.size(); t++) if(use_labs[t]) {
            Prikaz p;
            p.pr = POSTAV;
            p.riadok = my_labs[t].first; p.stlpec = my_labs[t].second;
            p.instrukcia = ((cc == 0)? ((rand() % 10 == 0)?(1):(state.eter - usable_labs_count + 1)) : (1)) ;
            instructions.push_back(p);
            cc++;
        }
    }

    // for every left (maybe handpicked) robot estimate the nearest interesting point,
    // pair robots to points and find the best moves to achieve that
    // use Edmonds later

    vector<vector<bool> > is_available_space(state.vyska, vector<bool>(state.sirka, true));
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (!is_square_valid(i, j, state, m)) is_available_space[i][j] = false;
            if (state.mapa[i][j].sila_robota > 0 && state.mapa[i][j].majitel == 0) is_available_space[i][j] = false; 
        }
    }

    random_shuffle(my_robots.begin(), my_robots.end());

    vector<vector<smer> > which_way_i_went(state.vyska, vector<smer>(state.sirka, TU));
    vector<int> order = {0,1,2,3};
    for (int t = 0; t < my_robots.size(); t++) {
        const int qx = my_robots[t].first;
        const int qy = my_robots[t].second;
        if (!unmoved[t]) continue;

        which_way_i_went.clear();
        which_way_i_went.resize(state.vyska, vector<smer>(state.sirka, TU));

        which_way_i_went[qx][qy] = DOLE;

        queue<pair<int, int> > Q;
        Q.push(my_robots[t]);

        bool found_interesting_spot = false;

        while(!Q.empty() && !found_interesting_spot) {
            int x = Q.front().first;
            int y = Q.front().second;
            Q.pop();

            // check by popping
            // place is good if
            // own not by me and robot strength is less than mine

            if ( (m.squares[x][y] == LAB || my_labs.size() > 0) && 
                        state.mapa[x][y].majitel != 0 && 
                        state.mapa[x][y].sila_robota < state.mapa[qx][qy].sila_robota &&
                        is_weaker_space(x, y, state.mapa[qx][qy].sila_robota, state, m)
                ) {
                // our sweet spot!
                while (abs(x - qx) + abs(y - qy) > 1) {
                    const int k = which_way_i_went[x][y]; 
                    x = x - dx[k];
                    y = y - dy[k];
                }

                Prikaz p;
                p.pr = POSUN;
                p.riadok = qx; p.stlpec = qy;
                p.instrukcia = which_way_i_went[x][y];

                is_available_space[x][y] = false;
                instructions.push_back(p);
                found_interesting_spot = true; 
                break;
            }


            // check by adding
            // place is safe if
            // own by me xor strength of robot is less than mine
            random_shuffle(order.begin(), order.end());
            for (int k = 0; k < 4; k++) {
                const int nx = x + dx[order[k]];
                const int ny = y + dy[order[k]];
                if (is_square_valid(nx, ny, state, m) && is_available_space[nx][ny] && 
                                which_way_i_went[nx][ny] == TU && 
                                state.mapa[nx][ny].sila_robota < state.mapa[qx][qy].sila_robota 
                    ) {

                    which_way_i_went[nx][ny] = static_cast<smer>(order[k]);
                    Q.push(make_pair(nx, ny));
                }
            }
        }


    }

    return instructions;
}

// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    cerr << "kolo " << _stav.kolo << " zistiTah() begin" << endl;
    vector<Prikaz> instrukcie;
    //  (sem patri vas kod)
    
    instrukcie = get_commands(_stav, _m);
    
    if(dbg) {
        cerr << "vysledne prikazy:" << endl;
        for (int i = 0; i < instrukcie.size(); i++) {
            cerr << instrukcie[i].pr << "\t" << instrukcie[i].riadok << "," << instrukcie[i].stlpec << "\t" << instrukcie[i].instrukcia << endl;
        }
    }

    cerr << "zistiTah() end" << endl;
    return instrukcie;
}


int main() {
    // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).
    
    unsigned int seed = time(NULL) * getpid();
    srand(seed);
    
    nacitaj(cin, _m);
    fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);
    inicializuj();

    while (cin.good()) {
        nacitaj(cin, _stav);
        uloz(cout, zistiTah());
//         cout << "2" << endl;   // 2 a flush = koniec odpovede
    }

    return 0;
}
