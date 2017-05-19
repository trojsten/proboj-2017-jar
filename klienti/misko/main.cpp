#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <set>
#include <queue>
using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"



set<pair<int, int>> obsadene;
mapa m;
masked_game_state stav;   // vzdy som hrac cislo 0


// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
    
    
  // (sem patri vas kod)
}

bool jeobsadene(int y, int x){
    /*for (int i=0; i<obsadene.size(); i++){
        if (obsadene[i].first==x && obsadene[i].second==y)
            return true;
    }
    return false;*/
    return obsadene.find(make_pair(y, x))!=obsadene.end();
}

bool ohrozene(int y, int x, int robot){
    if (y>0 && stav.mapa[y-1][x].majitel!=0 && stav.mapa[y-1][x].sila_robota>robot)
        return true;
    if (x>0 && stav.mapa[y][x-1].majitel!=0 && stav.mapa[y][x-1].sila_robota>robot)
        return true;
    if (y<stav.vyska-1 && stav.mapa[y+1][x].majitel!=0 && stav.mapa[y+1][x].sila_robota>robot)
        return true;
    if (x<stav.sirka-1 && stav.mapa[y][x+1].majitel!=0 && stav.mapa[y][x+1].sila_robota>robot)
        return true;
    return false;
}

bool naozajChcem(int y, int x, int robot){
    return y>=0 && y<stav.vyska && x>=0 && x<stav.sirka &&
            m.squares[y][x]==MESTO && stav.mapa[y][x].majitel!=0 && stav.mapa[y][x].sila_robota<robot && !jeobsadene(y, x);
}

bool bana(int y, int x, int robot){
    return y>=0 && y<stav.vyska && x>=0 && x<stav.sirka &&
            m.squares[y][x]==LAB && stav.mapa[y][x].majitel!=0 && stav.mapa[y][x].sila_robota<robot && !jeobsadene(y, x);
}

bool zabijem(int y, int x, int robot){
    return y>=0 && y<stav.vyska && x>=0 && x<stav.sirka && m.squares[y][x]!=KAMEN && stav.mapa[y][x].majitel!=0
            && stav.mapa[y][x].sila_robota<robot && stav.mapa[y][x].sila_robota>0 && !jeobsadene(y, x);
}

bool ovladnem(int y, int x, int robot){
    return y>=0 && y<stav.vyska && x>=0 && x<stav.sirka && m.squares[y][x]!=KAMEN && stav.mapa[y][x].majitel!=0 && stav.mapa[y][x].sila_robota<robot
            && !jeobsadene(y, x) && !ohrozene(y, x, robot);
}

bool mozem(int y, int x, int robot){
    return y>=0 && y<stav.vyska && x>=0 && x<stav.sirka && m.squares[y][x]!=KAMEN && stav.mapa[y][x].sila_robota<robot
            && !jeobsadene(y, x);
}

struct Stav{
    int y;
    int x;
    int vzd;
    int smer;
    
    Stav(int a, int b, int c, int d){
        y=a;
        x=b;
        vzd=c;
        smer=d;
    }
};

int blizkeVolno(int y, int x, int robot){
    int n=40;
    int visited [2*n+1][2*n+1];
    for (int i=0; i<2*n+1; i++)
        for (int j=0; j<2*n+1; j++)
            visited[i][j]=-10;
    
    queue<Stav> fronta;
    struct Stav s (y, x, 0, TU);
    fronta.push(s);
    
    while (! fronta.empty()){
        struct Stav top = fronta.front();
        fronta.pop();
        if (top.vzd>n)
            break;
        int y2=top.y;
        int x2=top.x;
        if (visited[y2-y+n][x2-x+n]!=-10)
            continue;
        visited[y2-y+n][x2-x+n]=top.smer;
        if (stav.mapa[y2][x2].majitel!=0 && stav.mapa[y2][x2].sila_robota<=robot)
            return top.smer;
        
        if (top.vzd<n){
            if (!visited[y2-y+n+1][x2-x+n] && y2<m.height-1 && m.squares[y2+1][x2]!=KAMEN){
                struct Stav dalsi (y2+1, x2, top.vzd+1, DOLE);
                fronta.push(dalsi);
            }
            if (!visited[y2-y+n-1][x2-x+n] && y2>0 && m.squares[y2-1][x2]!=KAMEN){
                struct Stav dalsi (y2-1, x2, top.vzd/1, HORE);
                fronta.push(dalsi);
            }
            if (!visited[y2-y+n][x2-x+n+1] && x2<m.width-1 && m.squares[y2][x2+1]!=KAMEN){
                struct Stav dalsi (y2, x2+1, top.vzd+1, VPRAVO);
                fronta.push(dalsi);
            }
            if (!visited[y2-y+n][x2-x+n-1] && x2>0 && m.squares[y2][x2-1]!=KAMEN){
                struct Stav dalsi (y2, x2-1, top.vzd+1, VLAVO);
                fronta.push(dalsi);
            }
        }
    }
    return TU;
}

bool contains(vector<int> v, int x){
    for (int i=0; i<v.size(); i++)
        if (v[i]==x)
            return true;
    
    return false;
}

// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    vector<Prikaz> instrukcie;
    obsadene.clear();
    
    int mesta = 0;
    for(int y=0; y<m.height; y++)
        for(int x=0; x<m.width; x++){
            if (m.squares[y][x]==MESTO && stav.mapa[y][x].majitel==0)
                mesta++;
        }
    
    for(int y=0; y<stav.vyska; y++)
        for(int x=0; x<stav.sirka; x++){
            if (stav.mapa[y][x].majitel==0 && m.squares[y][x]==LAB && !jeobsadene(y, x) && stav.mapa[y][x].sila_robota<=0){
                if (mesta>0 && rand()%(mesta*8)==1){
                    Prikaz p;
                    p.pr = POSTAV;
                    p.riadok = y;
                    p.stlpec = x;
                    p.instrukcia = stav.eter*3/4;
                    stav.eter -= p.instrukcia;
                    instrukcie.push_back(p);
                }
                else if (mesta>0 && rand()%(mesta*3)==1){
                    Prikaz p;
                    p.pr = POSTAV;
                    p.riadok = y;
                    p.stlpec = x;
                    p.instrukcia = stav.eter*2/5;
                    stav.eter -= p.instrukcia;
                    instrukcie.push_back(p);
                }
                else if (stav.eter>=1){
                    Prikaz p;
                    p.pr = POSTAV;
                    p.riadok = y;
                    p.stlpec = x;
                    p.instrukcia = (int)log(stav.eter)+1;
                    stav.eter -= p.instrukcia;
                    instrukcie.push_back(p);
                }
            }
            if (stav.mapa[y][x].majitel==0 && stav.mapa[y][x].sila_robota>0){
                Prikaz p;
                p.pr = POSUN;
                p.riadok = y;
                p.stlpec = x;
                p.instrukcia = TU;
                
                if (naozajChcem(y-1, x, stav.mapa[y][x].sila_robota))
                    p.instrukcia = HORE;
                else if (naozajChcem(y+1, x, stav.mapa[y][x].sila_robota))
                    p.instrukcia = DOLE;
                else if (naozajChcem(y, x-1, stav.mapa[y][x].sila_robota))
                    p.instrukcia = VLAVO;
                else if (naozajChcem(y, x+1, stav.mapa[y][x].sila_robota))
                    p.instrukcia = VPRAVO;
                
                else if (bana(y-1, x, stav.mapa[y][x].sila_robota))
                    p.instrukcia = HORE;
                else if (bana(y+1, x, stav.mapa[y][x].sila_robota))
                    p.instrukcia = DOLE;
                else if (bana(y, x-1, stav.mapa[y][x].sila_robota))
                    p.instrukcia = VLAVO;
                else if (bana(y, x+1, stav.mapa[y][x].sila_robota))
                    p.instrukcia = VPRAVO;
                
                else if (zabijem(y-1, x, stav.mapa[y][x].sila_robota) && rand()%6!=0)
                    p.instrukcia = HORE;
                else if (zabijem(y+1, x, stav.mapa[y][x].sila_robota) && rand()%6!=0)
                    p.instrukcia = DOLE;
                else if (zabijem(y, x-1, stav.mapa[y][x].sila_robota) && rand()%6!=0)
                    p.instrukcia = VLAVO;
                else if (zabijem(y, x+1, stav.mapa[y][x].sila_robota) && rand()%6!=0)
                    p.instrukcia = VPRAVO;
                
                else {
                    vector<int> kam;
                    
                    if (ovladnem(y-1, x, stav.mapa[y][x].sila_robota))
                        kam.push_back(HORE);
                    if (ovladnem(y+1, x, stav.mapa[y][x].sila_robota))
                        kam.push_back(DOLE);
                    if (ovladnem(y, x-1, stav.mapa[y][x].sila_robota))
                        kam.push_back(VLAVO);
                    if (ovladnem(y, x+1, stav.mapa[y][x].sila_robota))
                        kam.push_back(VPRAVO);
                    
                    if (kam.size()>0)
                        p.instrukcia = kam[rand()%kam.size()];
                    else {
                        if (mozem(y-1, x, stav.mapa[y][x].sila_robota))
                            kam.push_back(HORE);
                        if (mozem(y+1, x, stav.mapa[y][x].sila_robota))
                            kam.push_back(DOLE);
                        if (mozem(y, x-1, stav.mapa[y][x].sila_robota))
                            kam.push_back(VLAVO);
                        if (mozem(y, x+1, stav.mapa[y][x].sila_robota))
                            kam.push_back(VPRAVO);
                        
                        int s = blizkeVolno(y, x, stav.mapa[y][x].sila_robota);
                        if (s==TU){
                            if (kam.size()>0)
                                p.instrukcia = kam[rand()%kam.size()];
                        }
                        else if (contains(kam, s))
                            p.instrukcia = s;
                        else if (kam.size()>0)
                            p.instrukcia = kam[rand()%kam.size()];
                    }
                }
                
                if (p.instrukcia==HORE)
                    obsadene.insert(make_pair(y-1, x));
                if (p.instrukcia==DOLE)
                    obsadene.insert(make_pair(y+1, x));
                if (p.instrukcia==VLAVO)
                    obsadene.insert(make_pair(y, x-1));
                if (p.instrukcia==VPRAVO)
                    obsadene.insert(make_pair(y, x+1));
                if (p.instrukcia==TU)
                    obsadene.insert(make_pair(y, x));
                if (p.instrukcia!=TU)
                    instrukcie.push_back(p);
            }
        }
    //  (sem patri vas kod)
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
