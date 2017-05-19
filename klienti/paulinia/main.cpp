// #include <../../run/media/paulinia/LENOVO/proboj-2017-jar/server/common.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

#define FOR(i,n)	for(int i=0;i<(int)n;i++)
#define FOB(i,n)	for(int i=n;i>=1;i--)
#define MP(x,y)	make_pair((x),(y))
#define ii pair<int, int>
#define lli long long int
#define ulli unsigned long long int
#define lili pair<lli, lli>
#ifdef EBUG
#define DBG	if(1)
#else
#define DBG	if(0)
#endif
#define SIZE(x) int(x.size())
const int infinity = 2000000999 / 2;
const long long int inff = 4000000000000000999;

mapa m;
masked_game_state stav;   // vzdy som hrac cislo 0

vector<vector<int> > distLab, distCity;
vector<vector<int> > tuzba_von, som_tam;

vector<int> DX = {0, 1, 0, -1};
vector<int> DY = {1, 0, -1, 0};

// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
    distCity.resize(m.height, vector<int>(m.width, -1));
    distLab.resize(m.height, vector<int>(m.width, -1));
    tuzba_von.resize(m.height, vector<int>(m.width, -1));
    som_tam.resize(m.height, vector<int>(m.width, -1));
    
    queue<ii> F, Q;
    
        FOR(i, m.height){
            FOR(j, m.width){
                if(m.squares[i][j] == LAB){
                    Q.push({i, j});
                    distLab[i][j] = 0;
                }
                if(m.squares[i][j] == MESTO){
                    distCity[i][j] = 0;
                    F.push({i, j});
                }
            }
        }
    
    DBG cerr << "Hotovo do front " << endl;
    
    while(F.size()){
        int x = F.front().first;
        int y = F.front().second;
        F.pop();
        
        FOR(i, 4){
            if(DX[i] + x < 0 || DX[i] + x >= m.width || DY[i] + y < 0 || DY[i] + y >= m.height) continue;
            if(m.squares[x + DX[i]][y + DY[i]] == KAMEN) continue;
            if(distCity[x + DX[i]][y + DY[i]] != -1) continue;
            F.push({x + DX[i], y + DY[i]});
            distCity[x + DX[i]][y + DY[i]] = distCity[x][y] + 1;
        }
        
    }
    
    while(Q.size()){
        int x = Q.front().first;
        int y = Q.front().second;
        Q.pop();
        
        FOR(i, 4){
            if(DX[i] + x < 0 || DX[i] + x >= m.width || DY[i] + y < 0 || DY[i] + y >= m.height) continue;
            if(m.squares[x + DX[i]][y + DY[i]] == KAMEN) continue;
            if(distLab[x + DX[i]][y + DY[i]] != -1) continue;
            Q.push({x + DX[i], y + DY[i]});
            distLab[x + DX[i]][y + DY[i]] = distLab[x][y] + 1;
        }
    }
    
}

const double enforce[] = {0.1, 0.35, 0.65};
const int probab_type[] = {2, 10, 1, 5};
const int probab_sum = 18;
const int probab = 7; // cislo <0, probab) nahodne pripocita k sile
const int koef[] = {3, 1, 7, 2, 4}; // city lab out attack defense
const int probab_attack = 5;
const int probab_defense = 15;
vector<smer> D = {VPRAVO, DOLE, VLAVO, HORE, TU};
queue<ii> Q;

int koef_attack(int x, int y, int sila){
    int K = 0;
    cerr << "KOEF ATTACK COMPUTING[" << x << "|" << y << endl;
    FOR(i, 4){
        cerr << "Chem it na " << x + DX[i] << " " << y + DY[i] << endl;
        if(stav.mapa[x + DX[i]][y + DY[i]].majitel == 0) continue;
        if(stav.mapa[x + DX[i]][y + DY[i]].sila_robota >= sila){
            K += koef[4] * (rand() % probab_defense);
        }
        else if(stav.mapa[x + DX[i]][y + DY[i]].sila_robota < sila){
            K += koef[3] * (rand() % probab_attack);
        }
    }
    return K;
}

// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    int money = stav.eter;
    vector<Prikaz> instrukcie;
    FOR(i, m.height){
        FOR(j, m.width){
            tuzba_von[i][j] = -1;
            if(stav.mapa[i][j].majitel != 0){
                if(m.squares[i][j] != KAMEN){
                    Q.push({i, j});
                    tuzba_von[i][j] = 0;
                }
                continue;
            }
        }
    }
    
    cerr << "WTF WTF" << endl;
    while(Q.size()){
        int x = Q.front().first;
        int y = Q.front().second;
        Q.pop();
        FOR(i, 4){
            if(m.squares[x + DX[i]][y + DY[i]] != KAMEN){
                if(tuzba_von[x + DX[i]][y + DY[i]] == -1){
                    tuzba_von[x + DX[i]][y + DY[i]] = tuzba_von[x][y] + 1;
                    Q.push({x + DX[i], y + DY[i]});
                }
            }
        }
    }
    cerr << "WTF SEGFAULT" << endl;
    FOR(i, m.height){
        FOR(j, m.width){
            if(stav.mapa[i][j].majitel != 0){
                continue;
            }
            if(stav.mapa[i][j].sila_robota > 0){
                int maxi = -500 + koef_attack(i, j, stav.mapa[i][j].sila_robota), maxdir = -1;
                FOR(l, 4){
                    int force = 0;
                    if(som_tam[i + DX[l]][j + DY[l]] == stav.kolo) continue;
                    if(m.squares[i + DX[l]][j + DY[l]] == KAMEN) continue;
                    if(stav.mapa[i + DX[l]][j + DY[l]].sila_robota){ // zatial sa vyhybam robotom
                        if(stav.mapa[i + DX[l]][j + DY[l]].majitel == 0 && m.squares[i + DX[l]][j + DY[l]] == LAB){
                            force = -20;
                        }
                        else if(stav.mapa[i + DX[l]][j + DY[l]].sila_robota > stav.mapa[i][j].sila_robota){
                            continue;
                        }
                    }
                    force = force - distLab[i + DX[l]][j + DY[l]] * koef[1] - distCity[i + DX[l]][j + DY[l]] * koef[0] - tuzba_von[i + DX[l]][j + DY[l]] * koef[2] + koef_attack(i + DX[l], j + DY[l], stav.mapa[i][j].sila_robota);
                    force += rand() % probab;
                    if(force > maxi){
                        maxi = force;
                        maxdir = l;
                    }
                }
                if(maxdir != -1){
                    if(m.squares[i + DX[maxdir]][j + DY[maxdir]] == LAB){
                        if(rand() % 2){
                            continue;
                        }
                    }
                    Prikaz P;
                    P.pr = POSUN;
                    P.instrukcia = D[maxdir];
                    P.riadok = i;
                    P.stlpec = j;
                    som_tam[i + DX[maxdir]][j + DY[maxdir]] = stav.kolo;
                    instrukcie.push_back(P);
                }
            }
            else if(m.squares[i][j] == LAB && som_tam[i][j] != stav.kolo){
                Prikaz P;
                P.pr = POSTAV;
                int type = rand() % probab_sum;
                P.instrukcia = 0;
                FOR(k, 4){
                    cerr << "probab " << type << endl;
                    if(type < probab_type[k]){
                        P.instrukcia = enforce[k] * money;
                        money -= P.instrukcia;
                        break;
                    }
                    type -= probab_type[k];
                }
                P.riadok = i;
                P.stlpec = j;
                if(P.instrukcia) instrukcie.push_back(P);
            }
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
        cerr << " ---"  << endl;
        uloz(cout, zistiTah());
    }

    return 0;
}
