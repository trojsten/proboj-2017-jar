#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>
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
                p.pr = POSUN;
                p.riadok = i;
                p.stlpec = j;
                
                vector<int> volne;
                if(m.squares[i-1][j]!=VODA && m.squares[i-1][j]!=KAMEN && stav.mapa[i-1][j].majitel!=0) volne.push_back(2);
                if(m.squares[i+1][j]!=VODA && m.squares[i+1][j]!=KAMEN && stav.mapa[i+1][j].majitel!=0) volne.push_back(3);
                if(m.squares[i][j+1]!=VODA && m.squares[i][j+1]!=KAMEN && stav.mapa[i][j+1].majitel!=0) volne.push_back(1);
                if(m.squares[i][j-1]!=VODA && m.squares[i][j-1]!=KAMEN && stav.mapa[i][j-1].majitel!=0) volne.push_back(0);
                
                if (volne.size() > 0) {
                    p.instrukcia = volne[rand()%(volne.size())];
                } else {
                    if(m.squares[i-1][j]!=VODA && m.squares[i-1][j]!=KAMEN) volne.push_back(2);
                    if(m.squares[i+1][j]!=VODA && m.squares[i+1][j]!=KAMEN) volne.push_back(3);
                    if(m.squares[i][j+1]!=VODA && m.squares[i][j+1]!=KAMEN) volne.push_back(1);
                    if(m.squares[i][j-1]!=VODA && m.squares[i][j-1]!=KAMEN) volne.push_back(0);
                    p.instrukcia = volne[rand()%(volne.size())];
                }
                
                if(p.instrukcia == 0 && stav.mapa[i][j-1].sila_robota != 0) {
                    continue;
                } else if(p.instrukcia == 1 && stav.mapa[i][j+1].sila_robota != 0) {
                    continue;
                } else if(p.instrukcia == 2 && stav.mapa[i-1][j].sila_robota != 0) {
                    continue;
                } else if(p.instrukcia == 3 && stav.mapa[i+1][j].sila_robota != 0) {
                    continue;
                }
                         
                instrukcie.push_back(p);
            }
            if (stav.mapa[i][j].majitel==0 && m.squares[i][j]==LAB && stav.mapa[i][j].sila_robota==0){
                Prikaz p;
                p.pr = POSTAV;
                p.riadok = i;
                p.stlpec = j;
                if(stav.kolo < 50) {
                    p.instrukcia = 1;
                } else {
                    p.instrukcia = rand()%(stav.eter+1);
                }
                stav.eter -= p.instrukcia;
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
  fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);
  inicializuj();

  while (cin.good()) {
    nacitaj(cin, stav);
    cerr<<"---"<<endl;
    uloz(cout, zistiTah());
  }

  return 0;
}
