#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

#define Tiles(j,i) for(int i=0; i<m.height; i++) for(int j=0; j<m.width; j++)



mapa m;
masked_game_state stav;   // vzdy som hrac cislo 0

struct Robot_ {
    int x, y;
    int dir;
    int tgt;
    bool operator== (const Robot_ &r) const {
        return x == r.x && y == r.y;
    }
};
typedef struct Robot_ Robot;

struct pair_hash {
    inline size_t operator()(const pair<int,int> & v) const {
        return v.first*31+v.second;
    }
};
struct robot_hash {
    inline size_t operator()(const Robot & v) const {
        return v.x*31+v.y;
    }
};

unordered_set<Robot, robot_hash> roboti;
unordered_set<pair<int,int>, pair_hash> lab;
int moje_laby;
unordered_map<pair<int,int>, int, pair_hash> r_block;


// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
  // (sem patri vas kod)
    Tiles(x,y)
        if (m.squares[y][x] == LAB)
            lab.insert(make_pair(x,y));
}

void newRobots();

int bfs(Robot * r, int tgt);

int runbfs(Robot * r, int tgt) {
    cerr << "tgt: " << tgt << " change to ";
    tgt = tgt && (moje_laby!=lab.size());
    cerr << tgt << endl;
    int res = bfs(r, tgt);
    if (res == TU && tgt) {
        res = bfs(r, 2);
        if (res == TU)
            res = bfs(r, 0);
    }
    if (res == TU && m.squares[r->y][r->x] == LAB) {
        if (m.squares[r->y][r->x-1] != KAMEN) return VLAVO;
        if (m.squares[r->y][r->x+1] != KAMEN) return VPRAVO;
        if (m.squares[r->y-1][r->x] != KAMEN) return HORE;
        if (m.squares[r->y+1][r->x] != KAMEN) return DOLE;
    }
    return res;
}

int bfs(Robot * r, int tgt) {
    unordered_set<pair<int,int>, pair_hash> O;
    queue<pair<pair<int,int>,pair<int,int>>> Q;
    int a = 15;
    O.insert(make_pair(r->x,r->y));
    while (a) {
        int b = rand()%4;
        if (~a & (1 << b)) continue;
        int i = r->x-(b==VLAVO)+(b==VPRAVO);
        int j = r->y-(b==HORE)+(b==DOLE);
        auto block = r_block.find(make_pair(i,j));
        if (block==r_block.end() || block->second < stav.mapa[r->y][r->x].sila_robota)
            Q.push(make_pair(make_pair(0, b), make_pair(i,j)));
        O.insert(make_pair(i,j));
        a &= ~(1 << b);
    }
    while (!Q.empty()) {
        auto t = Q.front(); Q.pop();
        //cerr << "r" << r->x << "," << r->y << " -> " << t.second.first << "," << t.second.second << " (" << t.first.second << ")" << endl;
        if (m.squares[t.second.second][t.second.first]==KAMEN ||
             (m.squares[t.second.second][t.second.first]==LAB && stav.mapa[t.second.second][t.second.first].majitel==0)) {
            //cerr << "kamen" << endl;
            continue;
        }
        switch (tgt) {
            case 0:
                if (stav.mapa[t.second.second][t.second.first].majitel!=0) {
                    cerr << "ciel " << t.second.first << "," << t.second.second << endl;
                    return t.first.second;
                } break;
            case 1:
                if (stav.mapa[t.second.second][t.second.first].majitel!=0 && m.squares[t.second.second][t.second.first] == LAB) {
                    cerr << "ciel " << t.second.first << "," << t.second.second << endl;
                    return t.first.second;
                } break;
            case 2:
                if (stav.mapa[t.second.second][t.second.first].majitel!=0 && m.squares[t.second.second][t.second.first] == MESTO) {
                    cerr << "ciel " << t.second.first << "," << t.second.second << endl;
                    return t.first.second;
                } break;
        }
        a = 15;
        while (a) {
            int b = rand()%4;
            if (~a & (1 << b)) continue;
            int x = t.second.first -(b==VLAVO) +(b==VPRAVO);
            int y = t.second.second-(b==HORE) +(b==DOLE);
            if (O.insert(make_pair(x,y)).second) {
                Q.push(make_pair(make_pair(t.first.first+1, t.first.second), make_pair(x,y)));
            }
            a &= ~(1 << b);
        }
    }
    return TU;
}

// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    cerr << "TAH " << stav.kolo << endl;
    
    r_block.erase(r_block.begin(),r_block.end());
    cerr << "nula: " << r_block.size() << endl;
    vector<Prikaz> instrukcie;
    
    moje_laby = 0;
    for (auto l : lab) {
        //cerr << "check lab " << lab[i].first << "," << lab[i].second << endl;
        moje_laby += stav.mapa[l.second][l.first].majitel==0;
    }
    //cerr << "labs: " << moje_laby << endl;
    
    newRobots();
    
    Tiles(x,y) {
        if (stav.mapa[y][x].majitel!=0 && stav.mapa[y][x].sila_robota>0) {
            if (stav.mapa[y][x].majitel!=0) r_block.insert(make_pair(make_pair(x,y),stav.mapa[y][x].sila_robota));
            if (stav.mapa[y-1][x].majitel!=0) r_block.insert(make_pair(make_pair(x,y-1),stav.mapa[y][x].sila_robota));
            if (stav.mapa[y+1][x].majitel!=0) r_block.insert(make_pair(make_pair(x,y+1),stav.mapa[y][x].sila_robota));
            if (stav.mapa[y][x-1].majitel!=0) r_block.insert(make_pair(make_pair(x-1,y),stav.mapa[y][x].sila_robota));
            if (stav.mapa[y][x+1].majitel!=0) r_block.insert(make_pair(make_pair(x+1,y),stav.mapa[y][x].sila_robota));
        }
    }
    
    unordered_set<Robot, robot_hash> roboti_new;
    for (auto r : roboti) {
        cerr << "robot " << r.x << "," << r.y << endl;
        if (stav.mapa[r.y][r.x].majitel!=0 || stav.mapa[r.y][r.x].sila_robota==0) {
            cerr << "died" << endl;
            continue;
        }
        if (!moje_laby) r.tgt = 1;
        r.dir = runbfs(&r, r.tgt);
        Prikaz p;
        p.pr = POSUN;
        p.riadok = r.y;
        p.stlpec = r.x;
        p.instrukcia = r.dir;
        instrukcie.push_back(p);
        switch ((smer)r.dir) {
            case VLAVO:
                r.x--;
                break;
            case VPRAVO:
                r.x++;
                break;
            case HORE:
                r.y--;
                break;
            case DOLE:
                r.y++;
                break;
        }
        r_block.insert(make_pair(make_pair(r.x,r.y),10));
        cerr << "smer " << r.dir << endl;
        roboti_new.insert(r);
    }
    roboti = roboti_new;
    
    vector<pair<int,int>> labb(lab.begin(),lab.end());
    random_shuffle(labb.begin(),labb.end());
    for (auto l : lab) {
        if (stav.mapa[l.second][l.first].majitel==0){
            Robot r;
            r.x = l.first; r.y = l.second; r.dir = rand()%4;
            r.tgt = 0;
            if (!(rand()%6)) r.tgt++;
            if (!(rand()%6)) r.tgt++;
            roboti.insert(r);
            Prikaz p;
            p.pr = POSTAV;
            p.riadok = l.second;
            p.stlpec = l.first;
            p.instrukcia = min(3+((r.tgt==1)*(rand()%4+1)*stav.eter/6),stav.eter);
            stav.eter -= p.instrukcia;
            instrukcie.push_back(p);
        }
    }
    return instrukcie;
}

void newRobots() {
    Tiles(x,y) {
        if (stav.mapa[y][x].majitel == 0 && /*m.squares[y][x]==LAB &&*/ stav.mapa[y][x].sila_robota > 0) {
            Robot r;
            r.x = x; r.y = y; r.dir = rand()%4;
            r.tgt = rand()%10;
            if (roboti.find(r)==roboti.end()) {
                cerr << "warn: new robot found at " << x << "," << y << endl;
                roboti.insert(r);
            }
        }
    }
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
