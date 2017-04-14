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
vector<postav_robota> zistiTah() {
    vector<postav_robota> instrukcie;
    postav_robota p;
    //  (sem patri vas kod)
    instrukcie.push_back(p);
    return instrukcie;
}


int main() {
  // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).

  unsigned int seed = time(NULL) * getpid();
  srand(seed);

  nacitaj(cin, m);
  fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);
  inicializuj();

  while (cin.good()) {
    nacitaj(cin, stav);
    uloz(cout, zistiTah());
    cout << "2" << endl;   // 2 a flush = koniec odpovede
  }

  return 0;
}
