#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"


mapa m;
masked_game_state stav;   // vzdy som hrac cislo 0


// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
  // (sem patri vas kod)
}


// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    vector<Prikaz> instrukcie;
    for(int i=0; i<stav.vyska; i++)
        for(int j=0; j<stav.sirka; j++){
            if (stav.mapa[i][j].majitel==0 && stav.mapa[i][j].sila_robota>0){
                Prikaz p;
                p.pr=POSUN;
                p.riadok=i;
                p.stlpec=j;
                p.instrukcia=rand()%4;
                instrukcie.push_back(p);
            }
            if (stav.mapa[i][j].majitel==0 && m.squares[i][j]==LAB &&stav.mapa[i][j].sila_robota==0){
                Prikaz p;
                p.pr=POSTAV;
                p.riadok=i;
                p.stlpec=j;
                p.instrukcia=rand()%(stav.zelezo+1);
                stav.zelezo-=p.instrukcia;
                instrukcie.push_back(p);
            }
        }
    return instrukcie;
}


int main() {
  // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).

  unsigned int seed = time(NULL) * getpid();
  srand(seed);

  nacitaj(cin, m);
  uloz(cerr, m);
  fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);
  inicializuj();

  while (cin.good()) {
    nacitaj(cin, stav);
    uloz(cerr, zistiTah());
    uloz(cout, zistiTah());
    cout << "2" << endl;   // 2 a flush = koniec odpovede
  }

  return 0;
}
