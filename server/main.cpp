#include <vector>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <algorithm>

using namespace std;

#include "util.h"
#include "common.h"
#include "update.h"
#include "klient.h"
#include "mapa.h"
#include "marshal.h"

const int MAX_CITAJ = 500000;
const int ROUND_TIME = 1000;

vector<Klient> klienti;

// Komentar z minuleho proboja. Mozno je nam to jedno.
// /* Komment z minuleho proboja. Mozno to chceme lepsie.
//  * // tato trapna funkcia existuje len kvoli inicializujSignaly()
//  * // btw, takto sa signal handling nerobi
//  * // len sa s tym nechceme babrat.
//  */
void zabiKlientov() {
    fprintf(stderr, "ukoncujem klientov\n");
    for (unsigned i=0; i<klienti.size(); i++) {
        klienti[i].zabi();
    }
}

template<class T> void checkOstream(T& s, string filename) {
    if (s.fail()) {
        fprintf(stderr, "neviem zapisovat do %s\n", filename.c_str());
        zabiKlientov();
        exit(1);
    }
}

bool validchar (char ch) {
    if ((ch >= '0' && ch <= '9') || (ch == '/') || (ch == '-') || (ch == '_')) {
        return false;
    }
    return true;
}

string last_valid_substr(string s) {
    int r = (int)s.size() - 1;
    while (r > 0 && !validchar(s[r - 1])) {
        r--;
    }
    if (r == 0 || (r < (int)s.size() &&s[r] != '/')) {
        r++;
    }
    int l = r - 1;
    while (l > 0 &&s[l - 1] != '/') {
        l--;
    }
    string last = s.substr(l, r - l);
    return last;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "usage: %s <zaznamovy-adresar> <mapa> <adresar-klienta-1> <adresar-klienta-2> ...\n", argv[0]);
        return 0;
    }
    
    unsigned int seed = time(NULL) * getpid();
    srand(seed);
    loguj("startujem server, seed je %u", seed);
    inicializujSignaly(zabiKlientov);
    
    string zaznAdr(argv[1]);
    if (!jeAdresar(zaznAdr)) {
        if (mkdir(zaznAdr.c_str(), 0777)) {
            fprintf(stderr, "main/mkdir: %s: %s\n", zaznAdr.c_str(), strerror(errno));
            exit(1);
        }
    } 
    else {
        fprintf(stderr, "main: prepisujem zaznamovy adresar: %s\n", zaznAdr.c_str());
    }
    
    string obsubor = zaznAdr+"/observation";
    fstream observationstream(obsubor.c_str(), fstream::out | fstream::trunc);
    checkOstream(observationstream, "observation");
    
    ofstream colorstream((zaznAdr+"/color").c_str());
    checkOstream(colorstream, zaznAdr+"/color");
    
    ofstream graphstream((zaznAdr+"/graph").c_str());
    checkOstream(graphstream, zaznAdr+"/graph");
    graphstream<<"kolo ";
//     random_shuffle(argv + 3, argv + argc);
    set<string> uzMena;
    
    int pocet_hracov = argc - 3;
    observationstream << pocet_hracov << endl;
    
    // nacitame mapu
    mapa gm;
    if (!gm.load(argv[2])) {
        fprintf(stderr, "main: nenacital som mapu\n");
        exit(1);
    }
    game_state gs(pocet_hracov, gm);
    gm.zamaskuj(1);
    uloz(observationstream, gm);
    
    for (int i = 3; i < argc; i++) {
        string klientAdr(argv[i]);
        string meno = last_valid_substr(klientAdr);
        graphstream<<meno<<" ";
        // meno klienta je cast za poslednym /, za ktorym nieco je
        
        //ak sa nejaky klient opakuje, odlis ich
        bool dajNahodnuFarbu = false;
        while (uzMena.count(meno)) {
            dajNahodnuFarbu = true;
            meno += "+";
        }
        uzMena.insert(meno);
        
        stringstream uvodne_data;
        gm.zamaskuj(0);
        uloz(uvodne_data, gm);
        uvodne_data<<endl;
        
        klienti.push_back(Klient(meno, uvodne_data.str(), klientAdr, zaznAdr));
        
        string farba;
        if (dajNahodnuFarbu) {
            for (int i = 0; i < 3; i++) {
                double cl = (9 + (double)(rand() % (2 * 9)) ) / (4 * 9);
                farba += to_string(cl) + " ";
            }
            farba += "1.0";
        }
        else {
            string clsubor = klientAdr + "/color";
            fstream clstream(clsubor.c_str(), fstream::in);
            getline(clstream, farba);
            clstream.close();
        }
        
        observationstream << meno << " " << farba << endl;
        colorstream<<farba<<endl;
        
    }
    colorstream.close();
    graphstream<<endl;
    
    
    // spusti klientov
    for (unsigned k = 0; k < klienti.size(); k++) {
        klienti[k].restartuj();
    }
    usleep(1000 * 1000ll);
    for (unsigned k = 0; k < klienti.size(); k++) {
        stringstream masked_state_str;
        masked_game_state mgs(gs, k);
        uloz(masked_state_str, mgs);
        masked_state_str << endl;
        klienti[k].posli(masked_state_str.str());

    }
    
    long long lasttime = gettime();
    
    bool iba_jeden=0;
    while (!iba_jeden && gs.round < MAX_POCET_KOL) {
        cerr << "tah " << gs.round << "\n";
        vector<instruction> commands;
        for (unsigned k = 0; k < klienti.size(); k++) {
            bool skoncil=0;
            int nacital=0;
            int pocet;
            while (gettime() - lasttime < ROUND_TIME && !skoncil) {
                if (!klienti[k].zije()) {
                    klienti[k].restartuj();
                    // klientovi posleme relevantne data
                    if (klienti[k].zije()) {
                        stringstream old_state_str;
                        masked_game_state mgs(gs,k);
                        uloz(old_state_str, mgs);
                        old_state_str<<endl;
                        klienti[k].posli(old_state_str.str());
                    }
                    continue;
                }
                string s=klienti[k].citaj(MAX_CITAJ);
                if(s=="")continue;
                else skoncil=1;
                stringstream riadky(s);
                
                if (riadky.eof()){
                    cerr<<"nic tu nieje"<<endl;
                    break;
                }
                riadky >> pocet;
                cerr<<"pocet "<<pocet<<endl;
                for(int i=0; i<pocet; i++) {
                    int cmd;
                    riadky >> cmd;
                    nacital++;
                    instruction prikaz;
                    prikaz.klient_id = k;
                    if (cmd == POSUN) {
                        prikaz.pr = POSUN;
                        int r=0, s=0;
                        riadky >> r >> s;
                        if (riadky.eof()) {
                            cerr << "Nesprávny príkaz " << k << ": žiadne suradnice robota za prikazom POSUN"<<r<<" "<<s << endl;
                            continue;
                        }
                        prikaz.riadok = r;
                        prikaz.stlpec = s;
                        int dir;
                        riadky >> dir;
                        if (riadky.eof()) {
                            cerr << "Nesprávny príkaz " << k << ": žiadny smer za príkazom POSUN" << endl;
                            continue;
                        }
                        //VLAVO, VPRAVO, HORE, DOLE
                        if (dir == VLAVO)
                            prikaz.sm = VLAVO;
                        else if (dir == VPRAVO)
                            prikaz.sm = VPRAVO;
                        else if (dir == HORE)
                            prikaz.sm = HORE;
                        else if (dir == DOLE)
                            prikaz.sm = DOLE;
                        else {
                            cerr << "Nesprávny príkaz " << k << ": nesprávny smer '" << dir << "'" << endl;
                        }
                    } 
                    else if (cmd == POSTAV) {
                        prikaz.pr=POSTAV;
                        int r, s;
                        riadky >> r >> s;
                        if (riadky.eof()) {
                            cerr << "Nesprávny príkaz " << k << ": žiadne suradnice labu za prikazom POSTAV" << endl;
                            continue;
                        }
                        prikaz.riadok = r;
                        prikaz.stlpec = s;
                        int sila;
                        riadky >> sila;
                        if (riadky.eof()) {
                            cerr << "Nesprávny príkaz " << k << ": žiadna sila za príkazom POSTAV" << endl;
                            continue;
                        }
                        prikaz.sila = sila;
                        
                    }
                    else {
                        cerr << "Nesprávny vstup " << k << ": nesprávny príkaz '" << cmd << "'" << endl;
                    }
                    commands.push_back(prikaz);
                }
            }
            if(nacital<pocet)cerr<<"nenacital som vsetko od "<<k<<endl;
        }
        vector<int> oldskore(gs.skore.size());
        int oldround = gs.round;
        for (unsigned i = 0; i < klienti.size(); i++) {
            oldskore[i]=gs.skore[i];
        }
        gs = update_game_state(gm, gs, commands);
        
        // TODO zabijeme mrtvych hracov
//         for (unsigned k = 0; k < klienti.size(); k++) {
//             if (!gs.players[k].alive) {
//                 klienti[k].zabi();
//             }
//         }

        stringstream observer_state_str;
        uloz(observer_state_str, gs);
        observer_state_str << endl;
        
        for (unsigned k = 0; k < klienti.size(); k++) {
            if (!klienti[k].zije()) {
                continue;
            }
            stringstream masked_state_str;
            masked_game_state mgs(gs,k);
            uloz(masked_state_str, mgs);
            masked_state_str << endl;
            klienti[k].posli(masked_state_str.str());
        }
        
        lasttime = gettime();
        
        observationstream << observer_state_str.str();
        
        graphstream << gs.round <<" ";
        for (unsigned i = 0; i < klienti.size(); i++) {
            graphstream << (gs.skore[i]-oldskore[i])/(gs.round-oldround) << " ";
        }
        graphstream <<endl;
        //TODO ukoncit hru ak zije iba jeden, alebo nie?
        vector<int> policok(gs.skore.size(),0);
        int spolu=0;
        for(int i=0; i<gs.height; i++){
            for(int j=0; j<gs.width; j++){
                if(gs.map[i][j].majitel!=-1){
                    policok[gs.map[i][j].majitel]++;
                    spolu++;
                }
            }
        }
        for(int i=0; i<policok.size(); i++){
            if(policok[i]==spolu)
                iba_jeden=1;            
        }
    }
    
    // cleanup
    observationstream.close();
    graphstream.close();
    zabiKlientov();
    
    ofstream rankstream((zaznAdr+"/rank").c_str());
    checkOstream(rankstream, zaznAdr+"/rank");
    for (unsigned i = 0; i < klienti.size(); i++) {
        rankstream << gs.skore[i] << "\n";
    }
    rankstream.close();
    
    // +- info o dlzke hry
    loguj("");
    
    return 0;
}
