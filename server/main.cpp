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

const auto MAX_CITAJ = 1024;
const auto ROUND_TIME = 40;

vector<Klient> klienti;

/* Komment z minuleho proboja. Mozno to chceme lepsie.
 * // tato trapna funkcia existuje len kvoli inicializujSignaly()
 * // btw, takto sa signal handling nerobi
 * // len sa s tym nechceme babrat.
 */
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
    
    // random_shuffle(argv + 3, argv + argc);
    set<string> uzMena;
    
    int pocet_hracov = argc - 3;
    observationstream << pocet_hracov << endl;
    
    for (int i = 3; i < argc; i++) {
        string klientAdr(argv[i]);
        string meno = last_valid_substr(klientAdr);
        
        // meno klienta je cast za poslednym /, za ktorym nieco je
        bool dajNahodnuFarbu = false;
        while (uzMena.count(meno)) {
            dajNahodnuFarbu = true;
            meno += "+";
        }
        uzMena.insert(meno);
        // posleme klientovi, kolkaty v poradi je
        string uvodneData = "hrac " + to_string(i - 3) + "\n";
        
        klienti.push_back(Klient(meno, uvodneData, klientAdr, zaznAdr));
        
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
    }
    
    // nacitame mapu
    game_map gm;
    if (!gm.load(argv[2])) {
        exit(1);
    }
    observationstream << gm.width << " " << gm.height << endl;
    
    game_state gs(pocet_hracov, gm);
    
    // spusti klientov
    for (unsigned k = 0; k < klienti.size(); k++) {
        klienti[k].restartuj();
    }
    
    stringstream state_str;
    uloz(state_str, gs);
    state_str << endl;
    for (unsigned k = 0; k < klienti.size(); k++) {
        klienti[k].posli(state_str.str());
    }
    
    usleep(1000 * 1000ll);
    
    long long lasttime = gettime();
    
    int last_rounds = -1;
    while (last_rounds != 0 && gs.round < MAX_ROUNDS) {
        cerr << "tah " << gs.round << "\n";
        vector<vector<player_command> > commands(klienti.size());
        
        while (gettime() - lasttime < ROUND_TIME) {
            // fetchujeme spravy klientov, ale este nesimulujeme kolo
            for (unsigned k = 0; k < klienti.size(); k++) {
                if (!gs.players[k].alive) {
                    continue;
                }
                if (!klienti[k].zije()) {
                    klienti[k].restartuj();
                    // klientovi posleme relevantne data
                    if (klienti[k].zije()) {
                        stringstream old_state_str;
                        uloz(old_state_str, gs);
                        klienti[k].posli(old_state_str.str());
                    }
                    continue;
                }
                
                stringstream riadky(klienti[k].citaj(MAX_CITAJ));
                
                while (true) {
                    string cmd;
                    riadky >> cmd;
                    if (riadky.eof()) break;
                    
                    if (cmd == "cd") {
                        string dir;
                        riadky >> dir;
                        if (riadky.eof()) {
                            cerr << "wrong input " << k << ": no dir after cd" << endl;
                            continue;
                        }
                        
                        cout << "hrac " << k << "'" << dir << "'" << endl;
                        if (dir == "LEFT") {
                            commands[k].push_back(player_command{LEFT});
                        } 
                        else if (dir == "RIGHT") {
                            commands[k].push_back(player_command{RIGHT});
                        } 
                        else if (dir == "UP") {
                            commands[k].push_back(player_command{UP});
                        } 
                        else if (dir == "DOWN") {
                            commands[k].push_back(player_command{DOWN});
                        } 
                        else {
                            cerr << "wrong input " << k << ": invalid dir '" << dir << "'" << endl;
                        }
                    } 
                    else {
                        cerr << "wrong input " << k << ": no such cmd '" << cmd << "'" << endl;
                    }
                }
            }
        }
        gs = update_game_state(gs, commands);
        
        // zabijeme mrtvych hracov
        for (unsigned k = 0; k < klienti.size(); k++) {
            if (!gs.players[k].alive) {
                klienti[k].zabi();
            }
        }
        
        stringstream state_str;
        uloz(state_str, gs);
        state_str << endl;
        
        for (unsigned k = 0; k < klienti.size(); k++) {
            if (!klienti[k].zije()) {
                continue;
            }
            if (commands[k].size() > 0) {
                klienti[k].posli(state_str.str());
            }
        }
        
        lasttime = gettime();
        
        observationstream << state_str.str();
        
        if (last_rounds < 0) {
            int remain_alive = 0;
            for (unsigned i = 0; i < gs.players.size(); i++) {
                if (gs.players[i].alive) remain_alive++;
            }
            
            if (remain_alive <= 1) {
                last_rounds = 8;
                
                for (unsigned i = 0; i < gs.players.size(); i++) {
                    if (gs.players[i].alive) {
                        gs.players[i].score += 47;
                    }
                }
            }
        } 
        else {
            last_rounds -= 1;
            if (last_rounds <= 0) break;
        }
    }
    
    // cleanup
    observationstream.close();
    zabiKlientov();
    
    ofstream rankstream((zaznAdr+"/rank").c_str());
    checkOstream(rankstream, zaznAdr+"/rank");
    for (unsigned i = 0; i < gs.players.size(); i++) {
        rankstream << /*klienti[i].meno <<*/ gs.players[i].score << "\n";
    }
    rankstream.close();
    
    // +- info o dlzke hry
    loguj("");
    
    return 0;
}
