#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <map>

#include "marshal.h"

using namespace std;

const int MAX_POCET_KOL = 1000;

enum smer {
    VLAVO, VPRAVO, HORE, DOLE
};

enum prikaz {
    POSUN, POSTAV
};

enum typ_stvorca {
    TRAVA, KAMEN, VODA, LAB, LAB_SPAWN, MESTO
};

struct postav_robota {
    prikaz pr = POSTAV;
    int lab_id;
    int sila;
};

struct pohni_robota {
    prikaz pr = POSUN;
    int id_robota;
    smer sm;
};

struct instruction {
    int klient_id;
    prikaz pr;
    union{
        struct{
            int lab_id;
            int sila;
        };
        struct{
            int id_robota; 
            smer sm;
        };
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
                                                (pr == POSUN) ? 
                                                    (id_robota<A.id_robota)
                                                :
                                                    (lab_id<A.lab_id)
                                            ):(0)
                                    )
                            ):(0)
                    );
    }
};
    
// struct bod {
//     int x, y;
//     
//     bool operator== (const bod A) const {
//         return x == A.x && y == A.y;
//     }
// };

struct stvorec {
    typ_stvorca typ;
    int majitel;
    int sila_robota;
};


// UCASTNICI: tento struct je pre vas nedolezity
struct mapa {
    int width, height, maxplayers;
    vector<vector<typ_stvorca>> squares;
    
    mapa(){}

    mapa(int width, int height);
    
    bool load (string filename);
};

struct game_state {
    int round;

    int width, height;

    vector<vector<stvorec> > map;

    game_state() {}
    game_state(int num_players, mapa gm);
};



#endif


#ifdef reflection
// tieto udaje pouziva marshal.cpp aby vedel ako tie struktury ukladat a nacitavat (je to magicke makro)

// reflection(bod)
//     member(x)
//     member(y)
// end()


reflection(postav_robota)
    member(pr)
    member(lab_id)
    member(sila)
end()

reflection(pohni_robota)
    member(pr)
    member(id_robota)
    member(sm)
end()

reflection(stvorec)
    member(typ)
    member(majitel)
    member(sila_robota)
end()

reflection(game_state)
    member(round)
    member(width)
    member(height)
    member(map)
end()

#endif
