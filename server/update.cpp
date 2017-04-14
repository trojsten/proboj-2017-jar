#include "update.h"
#include <iostream>
#include <algorithm>

using namespace std;


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


game_state update_game_state(mapa gm, game_state gs, vector<instruction> commands) {
    game_state new_gs = gs;
    new_gs.round++;
    cerr<<"update na kolo "<<new_gs.round<<endl;
    sort(commands.begin(), commands.end());
    vector<vector<multisquare> > map;
    map.resize(gs.height);
    for(int i=0; i<gs.height; i++){
        map[i].resize(gs.width);
        for(int j=0; j<gs.width; j++){
            map[i][j].typ=gm.squares[i][j];
            map[i][j].bol_tu.majitel = gs.map[i][j].majitel;
            map[i][j].bol_tu.sila = gs.map[i][j].sila_robota;
            map[i][j].bol_tu.sm = TU;
        }
    }
    
    //nastav nove pozicie
    for(int i=0; i<commands.size(); i++){
        instruction cmd = commands[i];
        //ak chce robit nieco dva krat preskoc
        if(i>0&&cmd.klient_id == commands[i-1].klient_id
              &&cmd.pr == commands[i-1].pr
              &&cmd.riadok == commands[i-1].riadok
              &&cmd.stlpec == commands[i-1].stlpec)
            continue;
        if(cmd.pr==POSUN){
            if(cmd.klient_id != map[cmd.riadok][cmd.stlpec].bol_tu.majitel){
                cerr<<cmd.klient_id<<" je chudák, chce hýbať robotom ktorý mu nepatrí"<<endl;
                continue;
            }
            if(map[cmd.riadok][cmd.stlpec].bol_tu.sila == 0){
                cerr<<cmd.klient_id<<" je chudák, chce hýbať robotom ktorý neexistuje"<<endl;
                continue;
            }
            if(cmd.sm == VLAVO){
                if(cmd.stlpec <= 0 || map[cmd.riadok][cmd.stlpec-1].typ == KAMEN){
                    cerr<<cmd.klient_id<<" je chudák, zabil sa o stenu"<<endl;
                    map[cmd.riadok][cmd.stlpec].bol_tu.sila=0;
                    continue;
                }
                map[cmd.riadok][cmd.stlpec].bol_tu.sm = VLAVO;
                map[cmd.riadok][cmd.stlpec-1].zprava = map[cmd.riadok][cmd.stlpec].bol_tu;
            }
            if(cmd.sm == VPRAVO){
                if(cmd.stlpec >= gs.width-1 || map[cmd.riadok][cmd.stlpec+1].typ == KAMEN){
                    cerr<<cmd.klient_id<<" je chudák, zabil sa o stenu"<<endl;
                    map[cmd.riadok][cmd.stlpec].bol_tu.sila=0;
                    continue;
                }
                map[cmd.riadok][cmd.stlpec].bol_tu.sm = VPRAVO;
                map[cmd.riadok][cmd.stlpec+1].zlava = map[cmd.riadok][cmd.stlpec].bol_tu;
            }
            if(cmd.sm == HORE){
                if(cmd.riadok <= 0 || map[cmd.riadok-1][cmd.stlpec].typ==KAMEN){
                    cerr<<cmd.klient_id<<" je chudák, zabil sa o stenu"<<endl;
                    map[cmd.riadok][cmd.stlpec].bol_tu.sila = 0;
                    continue;
                }
                map[cmd.riadok][cmd.stlpec].bol_tu.sm = HORE;
                map[cmd.riadok-1][cmd.stlpec].zdola = map[cmd.riadok][cmd.stlpec].bol_tu;
            }
            if(cmd.sm == DOLE){
                if(cmd.riadok >= gs.height-1 || map[cmd.riadok+1][cmd.stlpec].typ==KAMEN){
                    cerr<<cmd.klient_id<<" je chudák, zabil sa o stenu"<<endl;
                    map[cmd.riadok][cmd.stlpec].bol_tu.sila = 0;
                    continue;
                }
                map[cmd.riadok][cmd.stlpec].bol_tu.sm = DOLE;
                map[cmd.riadok+1][cmd.stlpec].zhora = map[cmd.riadok][cmd.stlpec].bol_tu;
            }
        }
        
    }
    
    //zabi krizujucich
    for(int i=0; i<gs.height; i++){
        for(int j=0; j<gs.width; j++){
            if(map[i][j].bol_tu.sila == 0) continue;
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
        if(cmd.pr == POSTAV){
            if(cmd.klient_id != map[cmd.riadok][cmd.stlpec].bol_tu.majitel){
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
            if(map[cmd.riadok][cmd.stlpec].bol_tu.sila>0){
                cerr<<cmd.klient_id<<" je chudák, chce postavit robota v labaku ktorý nieje prazdny"<<endl;
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
            
            if(bol_tu.sm!=TU)bol_tu.sila=0;
            
            int ostane = max(max(bol_tu.sila, zhora.sila), max(zdola.sila, max(zprava.sila, zlava.sila)));
            if (ostane==0){map[i][j].bol_tu = bol_tu; continue;}
            int pocet=0;
            if(bol_tu.sila==ostane)pocet++; 
            if(zhora.sila==ostane)pocet++;
            if(zdola.sila==ostane)pocet++;
            if(zprava.sila==ostane)pocet++;
            if(zlava.sila==ostane)pocet++;
            int vitaz = rand()%pocet;
            int bolo=0;

            if(bol_tu.sila==ostane){bolo++;}
            if(zhora.sila==ostane){if(bolo==vitaz){bol_tu=zhora;} bolo++;}
            if(zdola.sila==ostane){if(bolo==vitaz){bol_tu=zdola;} bolo++;}
            if(zprava.sila==ostane){if(bolo==vitaz){bol_tu=zprava;} bolo++;}
            if(zlava.sila==ostane){if(bolo==vitaz){bol_tu=zlava;} bolo++;}
            
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
             if(map[i][j].typ == MESTO && map[i][j].bol_tu.majitel != -1)miest[map[i][j].bol_tu.majitel]++;
             if(map[i][j].typ == LAB && map[i][j].bol_tu.majitel != -1)labov[map[i][j].bol_tu.majitel]++;
             if(map[i][j].bol_tu.majitel != -1)policok[map[i][j].bol_tu.majitel]++;
         }
     }
     for(int i=0; i<new_gs.zelezo.size(); i++){
         new_gs.zelezo[i]+=labov[i];
         new_gs.zelezo[i]+=miest[i];
         new_gs.zelezo[i]+=policok[i]/9;
     }
    cerr<<"hotovo"<<endl;
    return new_gs;
}
