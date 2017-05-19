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
    
    vector<vector<int>> ideal(stav.vyska,vector<int>(stav.sirka,TU));
    vector<pair<int,int>> q(0);
    for(int i=0; i<stav.vyska; i++)
        for(int j=0; j<stav.sirka; j++){
            if(stav.mapa[i][j].majitel!=0 && m.squares[i][j]!=KAMEN){
                if(stav.mapa[i-1][j].majitel==0 && ideal[i-1][j]==TU){
                    ideal[i-1][j]=DOLE;
                    q.push_back(pair<int,int>(i-1,j));
                }
                if(stav.mapa[i+1][j].majitel==0 && ideal[i+1][j]==TU){
                    ideal[i+1][j]=HORE;
                    q.push_back(pair<int,int>(i+1,j));
                }
                if(stav.mapa[i][j+1].majitel==0 && ideal[i][j+1]==TU){
                    ideal[i][j+1]=VLAVO;
                    q.push_back(pair<int,int>(i,j+1));
                }
                if(stav.mapa[i][j-1].majitel==0 && ideal[i][j-1]==TU){
                    ideal[i][j-1]=VPRAVO;
                    q.push_back(pair<int,int>(i,j-1));
                }
            }    
        }
    for(int k=0; k<q.size();k++){
        int i=q[k].first,j=q[k].second;
        if(stav.mapa[i-1][j].majitel==0 && ideal[i-1][j]==TU){
                    ideal[i-1][j]=DOLE;
                    q.push_back(pair<int,int>(i-1,j));
                }
                if(stav.mapa[i+1][j].majitel==0 && ideal[i+1][j]==TU){
                    ideal[i+1][j]=HORE;
                    q.push_back(pair<int,int>(i+1,j));
                }
                if(stav.mapa[i][j+1].majitel==0 && ideal[i][j+1]==TU){
                    ideal[i][j+1]=VLAVO;
                    q.push_back(pair<int,int>(i,j+1));
                }
                if(stav.mapa[i][j-1].majitel==0 && ideal[i][j-1]==TU){
                    ideal[i][j-1]=VPRAVO;
                    q.push_back(pair<int,int>(i,j-1));
                }
    }
    
    
    vector<vector<bool>> obsadene(stav.vyska,vector<bool>(stav.sirka,false));
    for(int i=0; i<stav.vyska; i++)
        for(int j=0; j<stav.sirka; j++){
            if (stav.mapa[i][j].majitel==0 && m.squares[i][j]==LAB){
                Prikaz p;
                p.pr = POSTAV;
                p.riadok = i;
                p.stlpec = j;
                obsadene[i][j]=true;
                p.instrukcia = (rand()%(stav.eter+1))*(rand()%(stav.eter+1))/stav.eter+1;
                stav.eter -= p.instrukcia;
                instrukcie.push_back(p);
            }
            if (stav.mapa[i][j].majitel==0 && stav.mapa[i][j].sila_robota>0){
                Prikaz p;
                p.pr = POSUN;
                p.riadok = i;
                p.stlpec = j;
                vector<int> mozebyt(0);
                bool jetam=false;
                if((not obsadene[i-1][j])&&not(m.squares[i-1][j]==KAMEN)){
                   mozebyt.push_back(HORE);if(ideal[i][j]==HORE)jetam=true;}
                if((not obsadene[i+1][j])&&not(m.squares[i+1][j]==KAMEN)){
                   mozebyt.push_back(DOLE);if(ideal[i][j]==DOLE)jetam=true;}
                if((not obsadene[i][j+1])&&not(m.squares[i][j+1]==KAMEN)){
                   mozebyt.push_back(VPRAVO);if(ideal[i][j]==VPRAVO)jetam=true;}
                if((not obsadene[i][j-1])&&not(m.squares[i][j-1]==KAMEN)){
                   mozebyt.push_back(VLAVO);if(ideal[i][j]==VLAVO)jetam=true;}
                p.instrukcia=TU;
                if(mozebyt.size()>0)p.instrukcia=mozebyt[rand()%mozebyt.size()];
                if(jetam)p.instrukcia=ideal[i][j];
                if(p.instrukcia==HORE)obsadene[i-1][j]=true;
                if(p.instrukcia==DOLE)obsadene[i+1][j]=true;
                if(p.instrukcia==VPRAVO)obsadene[i][j+1]=true;
                if(p.instrukcia==VLAVO)obsadene[i][j-1]=true;
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
