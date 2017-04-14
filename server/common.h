#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <map>

#include "marshal.h"

using namespace std;

const int MAX_POCET_KOL = 1000;

enum smer {
    VLAVO, VPRAVO, HORE, DOLE, TU
};

enum prikaz {
    POSUN, POSTAV, KONIEC
};

enum typ_stvorca {
    TRAVA, KAMEN, VODA, LAB, MESTO, LAB_SPAWN
};

struct postav_robota {
    prikaz pr = POSTAV;
    int riadok;
    int stlpec;
    int sila;
};

struct pohni_robota {
    prikaz pr = POSUN;
    int riadok;
    int stlpec;
    smer sm;
};

union Prikaz{
    postav_robota p;
    pohni_robota q;
    Prikaz(postav_robota a){p=a;}
    Prikaz(pohni_robota a){q=a;}
};

struct instruction {
    int klient_id;
    prikaz pr;
    int riadok;
    int stlpec;
    union{
        int sila;
        smer sm;
    };
    
    bool operator<(const instruction A) const {
        return  (klient_id < A.klient_id) ? 
                    (1)
                : 
                    (
                        (klient_id == A.klient_id) ? 
                            (
                                (pr<A.pr) ? 
                                    (1) 
                                : 
                                    (
                                        (pr == A.pr) ? 
                                            (
                                                (riadok<A.riadok) ?
                                                        (1)
                                                    :
                                                        (
                                                            (riadok==A.riadok) ?
                                                                (stlpec<A.stlpec)
                                                            :(0)
                                                        )
                                            ):(0)
                                    )
                            ):(0)
                    );
    }
};


struct stvorec {
    int majitel = -1;
    int sila_robota;
};


// UCASTNICI: tento struct je pre vas nedolezity
struct mapa {
    int width, height, maxplayers;
    vector<vector<typ_stvorca>> squares;
    
    mapa(){}
    
    mapa(int width, int height);
    
    bool load (string filename);
    void zamaskuj(bool voda);
};


struct game_state {
    int round;
    int width, height;
    vector<int> zelezo;
    vector<vector<stvorec> > map;

    game_state() {}
    game_state(int num_players, mapa gm);
};

struct masked_game_state {
    int round;
    int width, height;
    int zelezo;
    vector<vector<stvorec> > map;
    masked_game_state(){}
    masked_game_state(game_state gs, int klient);
};



#endif

#ifdef reflectenum

reflectenum(smer)
reflectenum(prikaz)
reflectenum(typ_stvorca)

#endif


#ifdef reflection
// tieto udaje pouziva marshal.cpp aby vedel ako tie struktury ukladat a nacitavat (je to magicke makro)

// reflection(bod)
//     member(x)
//     member(y)
// end()


reflection(postav_robota)
    member(pr)
    member(riadok)
    member(stlpec)
    member(sila)
end()

reflection(pohni_robota)
    member(pr)
    member(riadok)
    member(stlpec)
    member(sm)
end()

reflection(stvorec)
    member(majitel)
    member(sila_robota)
end()

reflection(mapa)
    member(width)
    member(height)
    member(squares)
end()

reflection(game_state)
    member(round)
    member(width)
    member(height)
    member(zelezo)
    member(map)
end()

reflection(masked_game_state)
    member(round)
    member(width)
    member(height)
    member(zelezo)
    member(map)
end()
#endif
