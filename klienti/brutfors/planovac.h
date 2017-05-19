#ifndef PLANOVAC_H
#define PLANOVAC_H
#include "common.h"

enum typ_verbovania
{
	SPAM, UTOK, OBRANA
};

struct suradnice
{
	int x, y;
	suradnice(int xx = -1, int yy = -1) : x(xx), y(yy)
	{
	}
};

struct robot
{
	int sila;
	int majitel;
	smer kam_pojdem;
	suradnice kde_som;
	vector<double> priority;
	double max_priority;
	
	robot(int s, int m, suradnice k) : sila(s), majitel(m), kde_som(k)
	{
		kam_pojdem = TU;
		priority = vector<double>(5, -1);
		max_priority = -1;
	}
	
	void zadaj_prikaz(smer s, double pr);
};

struct lab
{
	suradnice kde_som;
	int idem_vyrobit;
	double priorita;
	typ_verbovania typ;
	
	lab(suradnice k) : kde_som(k), idem_vyrobit(-1)
	{
	}
	
	void zadaj_prikaz(int s, double pr, typ_verbovania t);
};

struct planovac_tahov
{
	vector<robot*> roboti;
	vector<robot*> nepriatelia;
	vector<pair<int, robot*> > vsetci_roboti;
	vector<lab> laby;
	double eter_obrana, eter_utok, eter_spam;
	int najsilnejsi_nepriatel;
	
	void update(masked_game_state stav, mapa m);
	vector<Prikaz> tahy();
	
	planovac_tahov();
	
	void prerozdel(double kolko, masked_game_state stav);
};


#endif