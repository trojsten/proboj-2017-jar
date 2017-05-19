#include "planovac.h"
#include <algorithm>

#define db(x) cerr << #x << " = " << x << endl

planovac_tahov::planovac_tahov()
{
	eter_utok = 0;
	eter_obrana = 0;
	eter_spam = 0;
	najsilnejsi_nepriatel = -1;
}

void planovac_tahov::update(masked_game_state stav, mapa m)
{
	for(int i=0; i<(int)nepriatelia.size(); i++)delete nepriatelia[i];
	for(int i=0; i<(int)roboti.size(); i++)delete roboti[i];
	nepriatelia.clear();
	roboti.clear();
	laby.clear();
	vsetci_roboti.clear();
	
	for(int y=0; y<stav.vyska; y++)
	{
		for(int x=0; x<stav.sirka; x++) 
		{
			if (stav.mapa[y][x].majitel == 0 && stav.mapa[y][x].sila_robota > 0) {
				roboti.push_back(new robot(stav.mapa[y][x].sila_robota, stav.mapa[y][x].majitel,  suradnice(x, y)));
				vsetci_roboti.push_back(make_pair(stav.mapa[y][x].sila_robota, roboti[roboti.size()-1]));
			}
			if (stav.mapa[y][x].majitel == 0 && m.squares[y][x] == LAB) {
				laby.push_back(lab(suradnice(x, y)));
			}
			if (stav.mapa[y][x].majitel > 0 && stav.mapa[y][x].sila_robota > 0)
			{
				nepriatelia.push_back(new robot(stav.mapa[y][x].sila_robota, stav.mapa[y][x].majitel, suradnice(x, y)));
				najsilnejsi_nepriatel = max(najsilnejsi_nepriatel, stav.mapa[y][x].sila_robota);
				vsetci_roboti.push_back(make_pair(stav.mapa[y][x].sila_robota, nepriatelia[nepriatelia.size()-1]));
			}
			
		}
	}
	
	double zvysny_eter = stav.eter - eter_utok - eter_obrana - eter_spam;
	prerozdel(zvysny_eter, stav);
	db(eter_utok);
	db(eter_obrana);
	db(eter_spam);
	sort(vsetci_roboti.rbegin(), vsetci_roboti.rend());
}

void planovac_tahov::prerozdel(double kolko, masked_game_state stav)
{
	if(kolko > 5)
	{
		eter_spam += kolko*0.3;
		//eter_obrana += kolko *0.1;
		eter_utok += kolko*0.7;
	}
	else
	{
		eter_spam += kolko;
	}
}

vector<Prikaz> planovac_tahov::tahy()
{
	vector<Prikaz> vysledok;
	for(int i=0; i<(int)roboti.size(); i++) {
		Prikaz p;
		p.pr = POSUN;
		p.riadok = roboti[i]->kde_som.y;
		p.stlpec = roboti[i]->kde_som.x;
		p.instrukcia = roboti[i]->kam_pojdem;
		if (roboti[i]->kam_pojdem != TU) {
			vysledok.push_back(p);
		}
	}
	for(int i=0; i<(int)laby.size(); i++)
	{
		if(laby[i].idem_vyrobit > 0)
		{
			Prikaz p;
			p.pr = POSTAV;
			p.riadok = laby[i].kde_som.y;
			p.stlpec = laby[i].kde_som.x;
			p.instrukcia = laby[i].idem_vyrobit;
			switch(laby[i].typ)
			{
				case UTOK:
				{
					eter_utok -= laby[i].idem_vyrobit;
					break;
				}
				case OBRANA:
				{
					eter_obrana -= laby[i].idem_vyrobit;
					break;
				}
				case SPAM:
				{
					eter_spam -= laby[i].idem_vyrobit;
					break;
				}
			}
			vysledok.push_back(p);
		}
	}
	return vysledok;
}

void robot::zadaj_prikaz(smer s, double pr)
{
	if(pr > priority[s])
	{
		priority[s] = pr;
	}
	if(pr > max_priority)
	{
		kam_pojdem = s;
		max_priority = pr;
	}
}

void lab::zadaj_prikaz(int s, double pr, typ_verbovania t)
{
	if(pr > priorita)
	{
		priorita = pr;
		idem_vyrobit = s;
		typ = t;
	}
}