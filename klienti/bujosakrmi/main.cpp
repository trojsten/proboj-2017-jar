#define DBG 0

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

#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define ull unsigned ll
#define ld long double
#define point complex<ll>
#define pll pair<ll, ll>
#define vll vector<ll>

#define INF 9223372036854775807
#define PI 3.141592653589793238
#define MOD 1000000009

#define sqr(x) ((x) * (x))
#define FOR(i, low, high) for(ll i = (low); i < (ll)(high); ++i)
#define MP(x, y) (make_pair((x), (y)))

mapa teren;
masked_game_state stav;   // vzdy som hrac cislo 0

typedef pair<ll, point> vazeny_bod;

struct Robot {
    point pos;
    ll sila;
    Robot(){}
    Robot(point a, ll b) {
        pos = a;
        sila = b;
    }
};

struct cmp {
    bool operator()(vazeny_bod &a, vazeny_bod &b) const {
        return a.first > b.first;
    }
};

struct cmp_robot {
    bool operator()(pair<ll, Robot> &a, pair<ll, Robot> &b) const {
        if(a.first == b.first)
            return a.second.sila > b.second.sila;
        return a.first > b.first;
    }
};

void printv(auto &v) {
    if(!v.size())
        return;
    FOR(i, 0, (signed ll)v.size()-1)
        cerr << v[i] << " ";
    cerr << v.back() << "\n";
}

bool moje(point p) {
    return stav.mapa[p.real()][p.imag()].majitel == 0;
}

typ_stvorca typ_terenu(point p) {
    return teren.squares[p.real()][p.imag()];
}

bool bound(point p) {
    return p.real() >= 0 && p.real() < stav.vyska && p.imag() >= 0 && p.imag() < stav.sirka; 
}

bool policko_ok(point p) {
    return bound(p) && typ_terenu(p) != KAMEN && !(moje(p) && typ_terenu(p) == LAB);
}

stvorec osadenie(point p) {
    return stav.mapa[p.real()][p.imag()];
}

point dirs[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

vector<point> najdi_okraje(bool moje_su_kamene=false) {
    if(DBG) cerr << "NAJDI_OKRAJE - hladam okraje\n";
    
    if(DBG) cerr << "\tpushujem okraje\n";
    queue<point> q;
    FOR(i, 0, stav.vyska) {
        q.push(point(i, 0));
        q.push(point(i, stav.sirka-1));
    }
    FOR(i, 0, stav.sirka) {
        q.push(point(0, i));
        q.push(point(stav.vyska-1, i));
    }
    
    if(DBG) cerr << "\thladam\n";
    vector< vector<bool> > seen(stav.vyska, vector<bool>(stav.sirka, false));
    vector<point> res;
    point pos, nex;
    while(!q.empty()) {
        pos = q.front();
        q.pop();
        
        if(seen[pos.real()][pos.imag()])
            continue;
        seen[pos.real()][pos.imag()] = true;
        
        if(policko_ok(pos) && (!moje_su_kamene || moje(pos))) {
            res.push_back(pos);
            continue;
        }
        
        for(point dir: dirs) {
            nex = pos + dir;
            if(!bound(nex) || seen[nex.real()][nex.imag()] || (moje_su_kamene && moje(nex)))
                continue;
            q.push(nex);
        }
    }
    if(DBG) cerr << "NAJDI_OKRAJE - koniec hladania\n";
    return res;
}

vector<vll> dijikstra(vector<point> &starty, vector<point> &okraje, bool moje_su_kamene=false) {
    if(DBG) cerr << "DIJIKSTRA - zacinam dijikstru\n";
    
    if(DBG) cerr << "\tpushujem okraje\n";
    priority_queue<vazeny_bod, vector<vazeny_bod>, cmp> q;
    for(point p: starty)
        if (!(moje_su_kamene && moje(p))) {
            ll modifier = -((!moje(p) && find(okraje.begin(), okraje.end(), p) != okraje.end()) * 1 +
                            (!moje(p) && typ_terenu(p) == LAB) * 10 +
                            (!moje(p) && typ_terenu(p) == MESTO) * 5);
            q.push(MP(modifier, p));
        }
    
    if(DBG) cerr << "\tzacinam hladat\n";
    vector< vector<bool> > seen(stav.vyska, vector<bool>(stav.sirka, false));
    vector<vll> d(stav.vyska, vll(stav.sirka, INF));
    point pos, nex;
    ll dist;
    while(!q.empty()) {
        tie(dist, pos) = q.top();
        q.pop();
        
        if(seen[pos.real()][pos.imag()] || (moje_su_kamene && moje(pos)))
            continue;
        seen[pos.real()][pos.imag()] = true;
        d[pos.real()][pos.imag()] = dist;
        
        for(point dir: dirs) {
            nex = pos + dir;
            if(!policko_ok(nex) || seen[nex.real()][nex.imag()] || (moje_su_kamene && moje(nex)))
                continue;
            q.push(MP(dist + 1 + 2 * moje(nex), nex));
        }
    }
    if(DBG) cerr << "DIJIKSTRA - koncim vyhladavanie\n";
    return d;
}

vector<vector<pll>> bfs_roboty(vector<point> &starty) {
    if(DBG) cerr << "DIJIKSTRA - zacinam dijikstru\n";
    
    if(DBG) cerr << "\tpushujem okraje\n";
    priority_queue<pair<ll, Robot>, vector<pair<ll, Robot>>, cmp_robot> q;
    for(point p: starty)
            q.push(MP(0, Robot(p, osadenie(p).sila_robota)));
        
    if(DBG) cerr << "\tzacinam hladat\n";
    vector< vector<bool> > seen(stav.vyska, vector<bool>(stav.sirka, false));
    vector<vector<pll>> res(stav.vyska, vector<pll>(stav.sirka, {INF, INF}));
    ll dist;
    Robot curr;
    point nex;
    while(!q.empty()) {
        tie(dist, curr) = q.top();
        q.pop();
        
        if(seen[curr.pos.real()][curr.pos.imag()])
            continue;
        seen[curr.pos.real()][curr.pos.imag()] = true;
        res[curr.pos.real()][curr.pos.imag()] = MP(dist, curr.sila);
        
        for(point dir: dirs) {
            nex = curr.pos + dir;
            if(!policko_ok(nex) || seen[nex.real()][nex.imag()])
                continue;
            q.push(MP(dist + 1, Robot(nex, curr.sila)));
        }
    }
    if(DBG) cerr << "DIJIKSTRA - koncim vyhladavanie\n";
    return res;
}

vector<point> najdi_roboty() {
    vector<point> res;
    FOR(i, 0, stav.vyska) {
        FOR(j, 0, stav.sirka) {
            if(moje(point(i, j)) && osadenie(point(i, j)).sila_robota > 0)
                res.push_back(point(i, j));
        }
    }
    return res;
}

vector<point> najdi_mesta() {
    vector<point> res;
    FOR(i, 0, stav.vyska) {
        FOR(j, 0, stav.sirka) {
            if(moje(point(i, j)) && typ_terenu(point(i, j)) == LAB && osadenie(point(i, j)).sila_robota == 0)
                res.push_back(point(i, j));
        }
    }
    return res;
}

vector<point> najdi_nemoje_mesta() {
    vector<point> res;
    FOR(i, 0, stav.vyska) {
        FOR(j, 0, stav.sirka) {
            if(!moje(point(i, j)) && typ_terenu(point(i, j)) == LAB)
                res.push_back(point(i, j));
        }
    }
    return res;
}

vector<point> najdi_volne_policka() {
    vector<point> res;
    FOR(i, 0, stav.vyska) {
        FOR(j, 0, stav.sirka) {
            if(!moje(point(i, j)) && typ_terenu(point(i, j)) != KAMEN)
                res.push_back(point(i, j));
        }
    }
    return res;
}

vector<point> najdi_protivnikov() {
    vector<point> res;
    FOR(i, 0, stav.vyska) {
        FOR(j, 0, stav.sirka) {
            point p=point(i, j);
            if(!moje(p) && osadenie(point(i, j)).sila_robota > 0)
                res.push_back(p);
        }
    }
    return res;
}

// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
  // (sem patri vas kod)
}

ll eteru_celkovo=0;
set<Robot> moji_roboti;
ll predchadzajuce_zelezo=0;

// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    cerr << "KOLO: " << stav.kolo << endl;
    vector<Prikaz> instrukcie;
    Prikaz p;
    vector<point> okraje=najdi_okraje();
    vector<point> volne_policka=najdi_volne_policka();
    vector<point> roboty=najdi_roboty();
    vector<point> mesta=najdi_mesta();
    vector<point> protivnici=najdi_protivnikov();
    vector<vector<pll>> vzdialenosti_od_protivnikov=bfs_roboty(protivnici);
    //vector<vll> vzdialenosti_od_okrajov=dijikstra(okraje, okraje, true);
    vector<vll> vzdialenosti_od_volnych_policok=dijikstra(volne_policka, okraje, false);
    
    if(DBG) {
        cerr << "OKRAJE\n";
        cerr << okraje.size()  << "\n";
        printv(okraje);

        cerr << "VZDIALENSOTI OD VOLNYCH POLICOK\n";
        FOR(i, 0, stav.vyska)
            printv(vzdialenosti_od_volnych_policok[i]);
            
        cerr << "ROBOTY\n";
        printv(roboty);
        cerr << "MESTA\n";
        printv(mesta);
    }
    
    pll mini;
    vector<pll> vmini;
    point pos;
    for(point robot: roboty) {
        p.pr = POSUN;
        p.riadok = robot.real();
        p.stlpec = robot.imag();
        
        ll dist, sila;
        vmini.clear(); vmini.push_back(MP(INF, INF));
        FOR(i, 0, 4){
            pos = robot + dirs[i];
            tie(dist, sila) = vzdialenosti_od_protivnikov[pos.real()][pos.imag()];
            if(sila < osadenie(robot).sila_robota && dist < 2 && dist <= vmini[0].first) {
                if(dist < vmini[0].first)
                    vmini.clear();
                vmini.push_back(MP(dist, i));
            }
            if(dist == INF)
                continue;
            
        }
        if(vmini[0].first == INF) {
            vmini.clear(); vmini.push_back(MP(INF, INF));
            FOR(i, 0, 4){
                pos = robot + dirs[i];
                ll dist=vzdialenosti_od_volnych_policok[pos.real()][pos.imag()];
                if(dist < vmini[0].first)
                    vmini.clear();
                if(dist <= vmini[0].first)
                    vmini.push_back(MP(dist, i));
                if(dist == INF)
                    continue;
                
            }
        }
        if(vmini[0].first < INF) {
            mini = vmini[rand()%vmini.size()];
            p.instrukcia = mini.second;
            point pos = robot + dirs[mini.second];
            //vzdialenosti_od_okrajov[pos.real()][pos.imag()] = INF;
            vzdialenosti_od_volnych_policok[pos.real()][pos.imag()] = INF;
            instrukcie.push_back(p);
        }
    }
    
    eteru_celkovo += stav.eter - predchadzajuce_zelezo;
    if(!mesta.empty()) {
        FOR(i, 0, mesta.size()) {
            point mesto = mesta[i];
            p.pr = POSTAV;
            p.riadok = mesto.real();
            p.stlpec = mesto.imag();
            p.instrukcia = ceil(stav.eter / 200.0);
            if(i == (ll)mesta.size()-1 && stav.kolo % 5 == 0 && stav.eter * 0.95 > (stav.sirka * stav.vyska / 8 / 9))
                p.instrukcia = stav.eter * 0.95;
            instrukcie.push_back(p);
            stav.eter -= p.instrukcia;
        }
    }
    
    predchadzajuce_zelezo = stav.eter;
    return instrukcie;
}


int main() {
    // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).
    
    unsigned int seed = time(NULL) * getpid();
    srand(seed);
    
    nacitaj(cin, teren);
    fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);
    inicializuj();

    while (cin.good()) {
        nacitaj(cin, stav);
        cerr<<"---"<<endl;
        uloz(cout, zistiTah());
    }

    return 0;
}
