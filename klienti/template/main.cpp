#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>

#include "common.h"

using namespace std;

int ja = -1;

int main() {
    srand(time(NULL));

//     string str;
//     cin >> str >> ja;
// 
     cerr << "ja"<<ja << endl;

    masked_game_state gs;

    while (true) {
	nacitaj(cin, gs);
    cerr<<"round "<<gs.round<<endl;
    cerr<<"rozmer "<<gs.width<<" "<<gs.height<<endl;
    cerr<<"zelezo "<<gs.zelezo<<endl;
    cerr<<"mapsize "<< gs.map.size()<<endl;
    uloz(cerr, gs);
    postav_robota p;
    //cerr<<"som nacital kolo "<<gs.round<<endl;
    for(int i=0; i<gs.height; i++){
        for(int j=0; j<gs.width; j++){
            if(gs.map[i][j].majitel==0){
                p.riadok=i;
                p.stlpec=j;
                break;
            }
        }
    }
    p.sila = 10;
    uloz(cerr, p);
    uloz(cout,p);
    cout<<endl;
// 	int x = gs.players[ja].position.x;
// 	int y = gs.players[ja].position.y;

// 	for (int x = 0; x < gs.width; x++) {
// 	    for (int y = 0; y < gs.height; y++) {
// 		char c = '.';
// 		if (gs.get_block(x, y).type == WALL) c = '#';
// 		else if (gs.get_block(x, y).crossed_by == ja) c = 'A';
// 		else if (gs.get_block(x, y).crossed_by != -1) c = 'B';
// 		cerr << c << " ";
// 	    }
// 	    cerr << endl;
// 	}
// 	cerr << endl;

// 	vector<string> dirs;
// 	if (x > 0 && gs.blocks[gs.block_index({x - 1, y})].crossed_by != ja) dirs.push_back("LEFT");
// 	if (x < gs.width - 1 && gs.blocks[gs.block_index({x + 1, y})].crossed_by != ja) dirs.push_back("RIGHT");
// 	if (y > 0 && gs.blocks[gs.block_index({x, y - 1})].crossed_by != ja) dirs.push_back("UP");
// 	if (y < gs.height - 1 && gs.blocks[gs.block_index({x, y + 1})].crossed_by != ja) dirs.push_back("DOWN");
// 
// 	if (dirs.size() > 0) {
// 	    cout << "cd " << dirs[rand() % dirs.size()] << endl;
// 	}
    }
}
