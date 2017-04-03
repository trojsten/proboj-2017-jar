#include "update.h"
#include <iostream>
#include <algorithm>

using namespace std;


// int own_territory(game_state& gs, int player) {
//     int points = 0;
//     return points;
// }

struct robot{
    int majitel = -1;
    int sila = 0;
    smer sm = TU;
};

struct multisquare{
    typ_stvorca typ;
    robot bol_tu;
    robot zhora;
    robot zdola;
    robot zprava;
    robot zlava;
};

// struct instruction {
//     int klient_id;
//     prikaz pr;
//     int riadok;
//     int stlpec;
//     union{
//         int sila;
//         smer sm;
//         };
// };
game_state update_game_state(game_state gs, vector<instruction> commands) {
    game_state new_gs = gs;
    
    new_gs.round++;
    sort(commands.begin(), commands.end());
    vector<vector<multisquare> > map;
    map.resize(gs.height);
    for(int i=0; i<gs.height; i++){
        map[i].resize(gs.width);
        for(int j=0; j<gs.width; j++){
            map[i][j].typ = gs.map[i][j].typ;
            map[i][j].bol_tu.majitel = gs.map[i][j].majitel;
            map[i][j].bol_tu.sila = gs.map[i][j].sila_robota;
            map[i][j].bol_tu.sm = TU;
        }
    }
    //nastav nove pozicie
    for(int i=0; i<commands.size(); i++){
        instruction cmd = commands[i];
        if(i>0&&cmd.klient_id==commands[i-1].klient_id
              &&cmd.pr==commands[i-1].pr
              &&cmd.riadok==commands[i-1].riadok
              &&cmd.stlpec==commands[i-1].stlpec)
            continue;
        if(cmd.pr==POSUN){
            if(cmd.klient_id!=map[cmd.riadok][cmd.stlpec].bol_tu.majitel){
                cerr<<cmd.klient_id<<" je chudák, chce hýbať robotom ktorý mu nepatrí"<<endl;
                continue;
            }
            if(map[cmd.riadok][cmd.stlpec].bol_tu.sila==0){
                cerr<<cmd.klient_id<<" je chudák, chce hýbať robotom ktorý neexistuje"<<endl;
                continue;
            }
            if(cmd.sm==VLAVO){
                if(cmd.stlpec<=0){
                    cerr<<cmd.klient_id<<" je chudák, zabil sa o stenu"<<endl;
                    map[cmd.riadok][cmd.stlpec].bol_tu.sm=VLAVO;
                    continue;
                }
                map[cmd.riadok][cmd.stlpec].bol_tu.sm=VLAVO;
                map[cmd.riadok][cmd.stlpec-1].zprava=map[cmd.riadok][cmd.stlpec].bol_tu;
            }
            if(cmd.sm==VPRAVO){
                if(cmd.stlpec>=gs.width-1){
                    cerr<<cmd.klient_id<<" je chudák, zabil sa o stenu"<<endl;
                    map[cmd.riadok][cmd.stlpec].bol_tu.sm=VPRAVO;
                    continue;
                }
                map[cmd.riadok][cmd.stlpec].bol_tu.sm=VPRAVO;
                map[cmd.riadok][cmd.stlpec+1].zprava=map[cmd.riadok][cmd.stlpec].bol_tu;
            }
            if(cmd.sm==HORE){
                if(cmd.riadok<=0){
                    cerr<<cmd.klient_id<<" je chudák, zabil sa o stenu"<<endl;
                    map[cmd.riadok][cmd.stlpec].bol_tu.sm=HORE;
                    continue;
                }
                map[cmd.riadok][cmd.stlpec].bol_tu.sm=HORE;
                map[cmd.riadok-1][cmd.stlpec].zprava=map[cmd.riadok][cmd.stlpec].bol_tu;
            }
            if(cmd.sm==DOLE){
                if(cmd.stlpec>=gs.height-1){
                    cerr<<cmd.klient_id<<" je chudák, zabil sa o stenu"<<endl;
                    map[cmd.riadok][cmd.stlpec].bol_tu.sm=DOLE;
                    continue;
                }
                map[cmd.riadok][cmd.stlpec].bol_tu.sm=DOLE;
                map[cmd.riadok+1][cmd.stlpec].zprava=map[cmd.riadok][cmd.stlpec].bol_tu;
            }
        }
        
    }
    //zabi krizujucich
    
//     struct robot{
//     int majitel = -1;
//     int sila = 0;
//     smer sm = TU;
// }
// 
// struct multisquare{
//     typ_stvorca typ;
//     robot bol_tu;
//     robot zhora;
//     robot zdola;
//     robot zprava;
//     robot zlava;
// }
//VLAVO VPRAVO HORE DOLE
    for(int i=0; i<gs.height; i++){
        for(int j=0; j<gs.width; j++){
            if(map[i][j].bol_tu.sila==0) continue;
            if(map[i][j].bol_tu.sm == VLAVO && map[i][j].zlava.sila != 0){
                if(map[i][j].bol_tu.sila > map[i][j].zlava.sila){
                    map[i][j].zlava.sila=0;
                }
                if(map[i][j].bol_tu.sila < map[i][j].zlava.sila){
                    map[i][j].bol_tu.sila=0;
                }
                if(map[i][j].bol_tu.sila == map[i][j].zlava.sila){
                    if(rand()%2){
                        map[i][j].zlava.sila=0;
                    }
                    else{
                        map[i][j].bol_tu.sila = 0;
                    }
                }
            }
            if(map[i][j].bol_tu.sm == VPRAVO && map[i][j].zprava.sila != 0){
                if(map[i][j].bol_tu.sila > map[i][j].zprava.sila){
                    map[i][j].zprava.sila=0;
                }
                if(map[i][j].bol_tu.sila < map[i][j].zprava.sila){
                    map[i][j].bol_tu.sila=0;
                }
                if(map[i][j].bol_tu.sila == map[i][j].zprava.sila){
                    if(rand()%2){
                        map[i][j].zprava.sila=0;
                    }
                    else{
                        map[i][j].bol_tu.sila = 0;
                    }
                }
            }
            if(map[i][j].bol_tu.sm == HORE && map[i][j].zhora.sila != 0){
                if(map[i][j].bol_tu.sila > map[i][j].zhora.sila){
                    map[i][j].zhora.sila=0;
                }
                if(map[i][j].bol_tu.sila < map[i][j].zhora.sila){
                    map[i][j].bol_tu.sila=0;
                }
                if(map[i][j].bol_tu.sila == map[i][j].zhora.sila){
                    if(rand()%2){
                        map[i][j].zhora.sila=0;
                    }
                    else{
                        map[i][j].bol_tu.sila = 0;
                    }
                }
            }
            if(map[i][j].bol_tu.sm == DOLE && map[i][j].zdola.sila != 0){
                if(map[i][j].bol_tu.sila > map[i][j].zdola.sila){
                    map[i][j].zdola.sila=0;
                }
                if(map[i][j].bol_tu.sila < map[i][j].zdola.sila){
                    map[i][j].bol_tu.sila=0;
                }
                if(map[i][j].bol_tu.sila == map[i][j].zdola.sila){
                    if(rand()%2){
                        map[i][j].zdola.sila=0;
                    }
                    else{
                        map[i][j].bol_tu.sila = 0;
                    }
                }
            }
        }
    }
    //postav vojakov v laboch
    
    for(int i=0; i<commands.size(); i++){
        instruction cmd = commands[i];
        if(i>0&&cmd.klient_id == commands[i-1].klient_id
              &&cmd.pr == commands[i-1].pr
              &&cmd.riadok == commands[i-1].riadok
              &&cmd.stlpec == commands[i-1].stlpec)
            continue;
        if(cmd.pr==POSTAV){
            if(cmd.klient_id!=map[cmd.riadok][cmd.stlpec].bol_tu.majitel){
                cerr<<cmd.klient_id<<" je chudák, chce postavit robota v labaku ktorý mu nepatrí"<<endl;
                continue;
            }
            if(cmd.sila<1){
                cerr<<cmd.klient_id<<" je chudák, chce postavit robota slabsieho ako 1"<<endl;
                continue;
            }
            if(cmd.sila> new_gs.zelezo[cmd.klient_id]){
                cerr<<cmd.klient_id<<" je chudák, chce postavit robota drahsieho ako si moze dovolit"<<endl;
                continue;
            }
            map[cmd.riadok][cmd.stlpec].bol_tu.sila = cmd.sila;
            new_gs.zelezo[cmd.klient_id]-=cmd.sila;
        }
        
    }
    
    //zabi nasobnich
    
    for(int i=0; i<gs.height; i++){
        for(int j=0; j<gs.width; j++){
            robot bol_tu=map[i][j].bol_tu;
            robot zhora=map[i][j].zhora;
            robot zdola=map[i][j].zdola;
            robot zprava=map[i][j].zprava;
            robot zlava=map[i][j].zlava;
            int ostane = max(max(bol_tu.sila, zhora.sila), max(zdola.sila, max(zprava.sila, zlava.sila)));
            int pocet=0;
            if(bol_tu.sila<ostane && bol_tu.sm==TU)bol_tu.sila=0;else pocet++;
            if(zhora.sila<ostane)zhora.sila=0;else pocet++;
            if(zdola.sila<ostane)zdola.sila=0;else pocet++;
            if(zprava.sila<ostane)zprava.sila=0;else pocet++;
            if(zlava.sila<ostane)zlava.sila=0;else pocet++;
            int vytaz =rand()%pocet;
            int bolo=0;
            if(bol_tu.sila==ostane && bol_tu.sm==TU){bolo++;}
            if(zhora.sila==ostane){if(bolo==vytaz){bol_tu=zhora;} bolo++;}
            if(zdola.sila==ostane){if(bolo==vytaz){bol_tu=zdola;} bolo++;}
            if(zprava.sila==ostane){if(bolo==vytaz){bol_tu=zprava;} bolo++;}
            if(zlava.sila==ostane){if(bolo==vytaz){bol_tu=zlava;} bolo++;}
            map[i][j].bol_tu = bol_tu;
        }
    }
    
    //pocitaj obsadene policka a generuj zelezo
    vector<int> policok(gs.zelezo.size(),0);
    vector<int> labov(gs.zelezo.size(),0);
    vector<int> miest(gs.zelezo.size(),0);
    
    for(int i=0; i<gs.height; i++){
        for(int j=0; j<gs.width; j++){
            new_gs.map[i][j].majitel = map[i][j].bol_tu.majitel;
            new_gs.map[i][j].sila_robota = map[i][j].bol_tu.sila;
            if(new_gs.map[i][j].typ==MESTO)miest[map[i][j].bol_tu.majitel]++;
            if(new_gs.map[i][j].typ==LAB)labov[map[i][j].bol_tu.majitel]++;
            policok[map[i][j].bol_tu.majitel]++;
        }
    }
    for(int i=0; i<new_gs.zelezo.size(); i++){
        new_gs.zelezo[i]+=labov[i];
        new_gs.zelezo[i]+=miest[i];
        new_gs.zelezo[i]+=policok[i]/9;
    }
    return new_gs;
}
