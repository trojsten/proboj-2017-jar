#include <fstream>
#include <algorithm>
#include "mapa.h"
#include "common.h"

using namespace std;

//game_state zamaskuj(int komu

#define chyba(...) (fprintf(stderr, __VA_ARGS__), false)



masked_game_state::masked_game_state(game_state gs, int klient) {
    zelezo = gs.zelezo[klient];
    kolo = gs.round;
    sirka = gs.width;
    vyska = gs.height;
    mapa.resize(vyska);

    for (int i = 0; i < vyska; i++) {
        mapa[i].resize(sirka);
        for (int j = 0; j < sirka; j++) {
            mapa[i][j] = gs.map[i][j];
            mapa[i][j].majitel = (gs.map[i][j].majitel==klient)?
                                    0
                                :(
                                    gs.map[i][j].majitel==0?
                                        klient
                                    :gs.map[i][j].majitel
                                );

            if((mapa[i][j].majitel!=0)
                    &&((i>0)?(gs.map[i-1][j].majitel!=klient):(1))
                    &&((j>0)?gs.map[i][j-1].majitel!=klient:1)
                    &&((i<vyska-1)?(gs.map[i+1][j].majitel!=klient):1)
                    &&((j<sirka-1)?(gs.map[i][j+1].majitel!=klient):1)){
                mapa[i][j].majitel = -1;
                mapa[i][j].sila_robota = -1;
            }
        }
    }
}

game_state::game_state(int num_players, mapa gm) {
    if(num_players>gm.maxplayers) chyba("Toľko hráčov sa na túto mapu nezmestí je tu %d miest",gm.maxplayers);
    zelezo.resize(num_players,0);
    skore.resize(num_players,0);
    round = 0;
    width = gm.width;
    height = gm.height;
    map.resize(height);

    for (int i = 0; i < height; i++) {
        map[i].resize(width);
        for (int j = 0; j < width; j++) {
            map[i][j] = {//gm.squares[i][j]==LAB_SPAWN?LAB:gm.squares[i][j], 
                        -1, 0};
        }
    }

    vector<pair<int, int>> spawns;
    for (int i = 0; i < gm.height; i++) {
        for (int j = 0; j < gm.width; j++) {
            if (gm.squares[i][j] == LAB_SPAWN) {
                spawns.push_back({i, j});
            }
        }
    }

    random_shuffle(spawns.begin(), spawns.end());
    for (int i = 0; i < num_players; i++) {
        map[spawns[i].first][spawns[i].second].majitel = i;
    }
}


mapa::mapa(int w, int h) {
    width = w;
    height = h;
    squares.resize(height);
    for (int i = 0; i < height; i++) {
        squares[i].resize(width);
        for (int j = 0; j < width; j++) {
            squares[i][j] = TRAVA;
        }
    }
}

void mapa::zamaskuj(bool voda) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (squares[i][j] == LAB_SPAWN)squares[i][j] = LAB;
            if (!voda && squares[i][j] == VODA) squares[i][j]=KAMEN;
        }
    }
}

bool mapa::load(string filename) {
    FILE *in = fopen(filename.c_str(), "r");
    if (!in) return chyba("neviem citat '%s'\n", filename.c_str());

    if (fgetc(in) != 'P') return chyba("'%s' ma zly format, chcem raw PPM\n", filename.c_str());
    if (fgetc(in) != '6') return chyba("'%s' ma zly format, chcem raw PPM\n", filename.c_str());

    // podporujeme komentare len medzi headerom a zvyskom (aj ked PPM standard umoznuje skoro kdekolvek)
    char c;
    fscanf(in, " %c", &c);
    while (c == '#') {
        while (c != '\n') c = fgetc(in);
        fscanf(in, " %c", &c);
    }
    ungetc(c, in);

    unsigned w, h, maxval;
    fscanf(in, "%u%u%u", &w, &h, &maxval);
    fgetc(in);
    if (maxval != 255) return chyba("'%s' ma zlu farebnu hlbku, podporujem len 24bpp\n", filename.c_str());

    width = w;
    height = h;
    maxplayers = 0;
    squares.clear();
    squares.resize(h);
    for (unsigned i = 0; i < h; i++) {
        squares[i].resize(w);
    }

    for (unsigned i = 0; i < h; i++) {
        for (unsigned j = 0; j < w; j++) {
            int r = fgetc(in);
            int g = fgetc(in);
            int b = fgetc(in);
            if (r == EOF || g == EOF || b == EOF) return chyba("necakany EOF pri citani '%s'\n", filename.c_str());
            // TRAVA, KAMEN, VODA, LAB, LAB_SPAWN, MESTO
            if (r == 255 && g == 255 && b == 255) squares[i][j] = TRAVA;
            else if (r == 0 && g == 0 && b == 0) squares[i][j] = KAMEN;
            else if (r == 0 && g == 0 && b == 255) squares[i][j] = VODA;
            else if (r == 255 && g == 0 && b == 0) squares[i][j] = LAB;
            else if (r == 0 && g == 255 && b == 0){ squares[i][j] = LAB_SPAWN; maxplayers++;}
            else if (r == 255 && g == 255 && b == 0) squares[i][j] = MESTO;
            else return chyba("zla farba %d,%d,%d na pozicii %d,%d v '%s'\n", r, g, b, i, j, filename.c_str());
        }
    }

    fclose(in);
    return true;
}
