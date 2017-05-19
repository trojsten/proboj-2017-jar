#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
using namespace std;

#define db(x) cerr << #x << " = " << x << endl

#include "common.h"
#include "marshal.h"
#include "update.h"
#include "planovac.h"

typedef pair<int, int> pii;
typedef pair<int, pii > pipii;
typedef pair<int, pipii > pipipii;

mapa m;
masked_game_state stav;   // vzdy som hrac cislo 0
planovac_tahov planovac;
const int dr[] = {0, 0, -1, 1, 0}, dc[] = {-1, 1, 0, 0, 0};

struct general
{
	virtual void daj_rozkazy() = 0;
};

vector<general*> velenie;

struct spammer : general
{
	void daj_rozkazy()
	{
		if(planovac.laby.size() == 0)return;
		int velkost = planovac.eter_spam / planovac.laby.size();
		if(velkost == 0)return;
		velkost = min(velkost, 5);
		int miniem = 0;
		for(int i=0; i<(int)planovac.laby.size(); i++)
		{
			int sila = rand()%velkost+1;
			planovac.laby[i].zadaj_prikaz(sila, 2, SPAM);
			miniem += sila;
		}
		planovac.eter_spam -= planovac.eter_spam - miniem;
	}
};

struct utocne_verbovanie : general
{
	int setrim_na;
	
	utocne_verbovanie()
	{
		setrim_na = -1;
	}
	
	void zvol_cenu(vector<int>& sily)
	{
		if(sily.size() == 0)
		{
			setrim_na = -1;
			return;
		}
		int typ = rand()%(rand()%6+1);
		double percentil = 0.5 + 0.1 * typ;
		setrim_na = sily[(sily.size()-1)* percentil+0.5]+1;
		db(setrim_na);
	}
	
	void daj_rozkazy()
	{
		vector<int> sily;
		for(int i=0; i<(int)planovac.nepriatelia.size(); i++)
		{
			sily.push_back(planovac.nepriatelia[i]->sila);
		}
		if(planovac.najsilnejsi_nepriatel > 0)sily.push_back(planovac.najsilnejsi_nepriatel);
		sort(sily.begin(), sily.end());
		int miniem = 0;
		vector<int> indexy;
		for(int i=0; i<(int)planovac.laby.size(); i++) indexy.push_back(i);
		random_shuffle(indexy.begin(), indexy.end());
		if(setrim_na == -1) zvol_cenu(sily);
		for(int i=0; i<(int)indexy.size() && setrim_na != -1 && miniem+setrim_na <= planovac.eter_utok; i++)
		{
			planovac.laby[indexy[i]].zadaj_prikaz(setrim_na, 5, UTOK);
			zvol_cenu(sily);
		}
	}
};

struct utok : general
{
	const int inf = 1023456789;
	void daj_rozkazy()
	{
		vector<vector<int> > dist(m.height, vector<int> (m.width, inf));
		priority_queue<pair<int, pair<int, int> >, vector<pair<int, pair<int, int> > >, greater<pair<int, pair<int, int> > > > halda;
		for(int y=0; y<m.height; y++)
		{
			for(int x=0; x<m.width; x++)
			{
				if(stav.mapa[y][x].majitel != 0) {
					int d = inf;
					switch (m.squares[y][x]) {
						case TRAVA: {
							d = 0;
							break;
						}
						case LAB: {
							d = -4;
							break;
						}
						case MESTO: {
							d = -2;
							break;
						}
						default: {
							break;
						}
					}
					if (stav.mapa[y][x].sila_robota > 0) {
						if (stav.mapa[y][x].sila_robota < 20) d -= 4;
						if (stav.mapa[y][x].sila_robota < 100) d -= 4;
						else d+=4;
					}
					halda.push(make_pair(d, make_pair(x, y)));
					dist[y][x] = d;
				}
			}
		}
		while(!halda.empty())
		{
			pair<int, pair<int, int> > cur = halda.top();
			halda.pop();
			int cx = cur.second.first, cy = cur.second.second;
			if(cur.first > dist[cy][cx])continue;
			for(int sm = 0; sm < 4; sm++)
			{
				int nx = cx + dc[sm], ny = cy + dr[sm];
				if(nx < 0 || nx >= m.width || ny < 0 || ny >= m.height) continue;
				if(m.squares[ny][nx] == KAMEN || m.squares[ny][nx] == VODA) continue;
				if(dist[ny][nx] > dist[cy][cx]+1)
				{
					dist[ny][nx] = dist[cy][cx]+1;
					halda.push(make_pair(dist[ny][nx], make_pair(nx, ny)));
				}
			}
		}

		for(int i=0; i<(int)planovac.roboti.size(); i++)
		{
			if(planovac.roboti[i]->sila < 20) continue;
			int bestd = inf;
			smer bests = TU;
			for(int sm=0; sm<4; sm++)
			{
				int nx = planovac.roboti[i]->kde_som.x + dc[sm];
				int ny = planovac.roboti[i]->kde_som.y + dr[sm];
				if(nx < 0 || nx >= m.width || ny < 0 || ny >= m.height)continue;
				if(dist[ny][nx] < bestd)
				{
					bestd = dist[ny][nx];
					bests = (smer)sm;
				}
			}
			if(bestd < inf) planovac.roboti[i]->zadaj_prikaz(bests, 3);
		}
		
		/*
		sort(planovac.nepriatelia.begin(), planovac.nepriatelia.end());
		int cena=0;
		int oplati=0;
		for(int i=0; i<(int)planovac.nepriatelia.size(); i++)
		{
			cena=planovac.nepriatelia[i]->sila;
			if(2*planovac.eter_utok/(cena+1)>i) oplati=cena+1;
		}
		
		int miniem = 0;
		for(int i=0; i<(int)indexy.size(); i++)
		{
			if(oplati>planovac.eter_utok-miniem) break;
			if(oplati) planovac.laby[indexy[i]].zadaj_prikaz(oplati, 5, UTOK);
			miniem += oplati;
			db(oplati);
		}*/
	}
};


struct obsadzovac : general
{
	const int inf = 1023456789;
	void daj_rozkazy()
	{
		vector<vector<int> > dist(m.height, vector<int> (m.width, inf));
		priority_queue<pipipii, vector<pipipii >, greater<pipipii > > halda;
		for(int y=0; y<m.height; y++)
		{
			for(int x=0; x<m.width; x++)
			{
				if(stav.mapa[y][x].majitel != 0)
				{
					int d = inf;
					switch(m.squares[y][x])
					{
						case TRAVA:
						{
							d = 0;
							break;
						}
						case LAB:
						{
							d = -6;
							break;
						}
						case MESTO:
						{
							d = -3;
							break;
						}
						default:
						{
							break;
						}
					}
					halda.push(make_pair(d, make_pair(rand(), make_pair(x, y))));
					dist[y][x] = d;
				}
			}
		}
		while(!halda.empty())
		{
			pipipii cur = halda.top();
			halda.pop();
			int cx = cur.second.second.first, cy = cur.second.second.second;
			if(cur.first > dist[cy][cx])continue;
			for(int sm = 0; sm < 4; sm++)
			{
				int nx = cx + dc[sm], ny = cy + dr[sm];
				if(nx < 0 || nx >= m.width || ny < 0 || ny >= m.height) continue;
				if(m.squares[ny][nx] == KAMEN || m.squares[ny][nx] == VODA) continue;
				if(m.squares[ny][nx] == LAB && stav.mapa[ny][nx].majitel == 0) continue;
				if(dist[ny][nx] > dist[cy][cx]+1)
				{
					dist[ny][nx] = dist[cy][cx]+1;
					halda.push(make_pair(dist[ny][nx], make_pair(rand(), make_pair(nx, ny))));
				}
			}
		}
		
		for(int i=0; i<(int)planovac.roboti.size(); i++)
		{
			int bestd = inf;
			smer bests = TU;
			for(int sm=0; sm<4; sm++)
			{
				int nx = planovac.roboti[i]->kde_som.x + dc[sm];
				int ny = planovac.roboti[i]->kde_som.y + dr[sm];
				if(nx < 0 || nx >= m.width || ny < 0 || ny >= m.height)continue;
				if(dist[ny][nx] < bestd)
				{
					bestd = dist[ny][nx];
					bests = (smer)sm;
				}
			}
			if(bestd < inf) planovac.roboti[i]->zadaj_prikaz(bests, 2);
		}
		
	}
};


struct pud_sebazachovy : general
{
	void daj_rozkazy()
	{
		vector<vector<bool> > obsadene(stav.vyska, vector<bool> (stav.sirka, 0));
		for(int y=0; y<m.height; y++)
		{
			for(int x=0; x<m.width; x++)
			{
				if(m.squares[y][x] == KAMEN || m.squares[y][x] == VODA) obsadene[y][x] = true;
				if(m.squares[y][x] == LAB && stav.mapa[y][x].majitel == 0) obsadene[y][x] = true;
			}
		}
		
		for (int i = 0; i < (int)planovac.vsetci_roboti.size(); ++i) {
			pair < int, robot * > akt = planovac.vsetci_roboti[i];
			if (akt.second->majitel != 0) {
				for (int j = 0; j < 5; ++j) {
					int x = akt.second->kde_som.x + dc[j], y = akt.second->kde_som.y + dr[j];
					if (x < 0 || x >= stav.sirka || y < 0 || y >= stav.vyska) continue;
					obsadene[y][x] = true;
				}
			} else {
				smer kam_pojdem = TU;
				int pr = -1;
				for (int j = 0; j < 5; ++j) {
					int x = akt.second->kde_som.x + dc[j], y = akt.second->kde_som.y + dr[j];
					if (x < 0 || x >= stav.sirka || y < 0 || y >= stav.vyska) continue;
					if (akt.second->priority[j] > pr) {
						if (!obsadene[y][x]) {
							pr = akt.second->priority[j];
							kam_pojdem = smer(j);
						}
					}
				}
				obsadene[akt.second->kde_som.y + dr[kam_pojdem]][akt.second->kde_som.x + dc[kam_pojdem]] = true;
				akt.second->zadaj_prikaz(kam_pojdem, 100);
			}
		}
	}
};

// main() zavola tuto funkciu, ked nacita mapu
void inicializuj()
{
	velenie.push_back(new spammer());
	velenie.push_back(new obsadzovac());
	velenie.push_back(new utocne_verbovanie());
	velenie.push_back(new utok());
	velenie.push_back(new pud_sebazachovy());
	// (sem patri vas kod)
}


// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah()
{
	planovac.update(stav, m);
	for(int i=0; i<(int)velenie.size(); i++) velenie[i]->daj_rozkazy();
	vector<Prikaz> tahy = planovac.tahy();
	return tahy;
	//vector<Prikaz> instrukcie;
	//  (sem patri vas kod)
	//return instrukcie;
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
