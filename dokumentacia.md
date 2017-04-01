
Čo je proboj
------------

Proboj je počítačová hra, ktorej hráčmi nie ste vy, ale programy, čo napíšete.


Zdrojáky
--------

Štandardný hráč, čiže klient (v adresári `klienti/template`), sa skladá z jediného
zdrojáku `main.cpp`. Ale môžete ho rozdeliť aj na viacero.

V serveri je tiež zopár zdrojákov, čo vás bude zaujímať.

- `common.h` obsahuje základné štruktúry, čo váš klient dostane k dispozícii.
- `update.cpp` a `update.h` obsahujú všetky herné konštanty, a tiež
  implementáciu väčšiny herných pravidiel, takže ak v pravidlách nie je niečo
  jasné, skúste sa tam pozrieť.
- v `main.cpp` sú tiež nejaké pravidlá (ako sa ťahá apod.), ale to je menej
  dôležité.

Kľudne si prečítajte aj ostatné zdrojáky, ja sa len poteším, ale pri kódení
vášho klienta vám asi nepomôžu.

Ako kódiť klienta
-----------------

Skopírujte obsah `klienti/template` do iného adresára a niečo v ňom nakóďte.

V koreni proboju spustite `make`, čím všetko skompilujete. (Ak váš klient nie je
vnútri `klienti`, nastavte v jeho `Makefile` správny `SERVERDIR` a spustite
`make` aj v ňom.)

Potom spustite `./server/server zaznamy/01 mapy/simple klienti/vasklient
klienti/vasklient klienti/hlupy` To spustí hru s troma hráčmi (vaším, druhým
vaším a hlúpym) a uloží záznam do `zaznamy/01`. Ten si môžete pozrieť tak,
že najprv zmeníte working directory na `Observer` (príkaz `cd Observer`)
a potom zadáte príkaz `java -jar ./dist/Proboj.jar ../zaznamy/01`.
Alebo zo probojoveho adresara zadate ./server

Server sa vášho klienta pýta, čo chce robiť. Pritom ale nečaká na vášho
klienta --- ak váš klient dlho premýšľa, tak keď konečne spraví ťah,
server už niekoľko ťahov stihol odsimulovať. Vždy, keď váš klient
odpovie serveru, mu ten pošle aktuálny stav hry.

Na začiatku hry dostane váš klient informácie o hre --- jeho poradové číslo,
a počiatočný stav hry. Tiež má pritom viac času, aby sa mohol
inicializovať --- server chvíľu počká, kým začne simulovať.

Keď server spustíte u vás, je to len na skúšku. Na hlavnom počítači to beží na
ostro. Je tam aj webové rozhranie, cez ktoré môžete uploadovať vašich klientov.
Uploadujú sa zdrojáky a tie sa potom skompilujú (konkrétne sa spustí `make
naserveri SERVERDIR=/adresar/kde/je/server`).


Aky je proboj
-------------

Ste had, nemáte ale chvost. Váš druhý koniec je v istom zmysle vaše územie.
Na začiatku začínate vo svojom území, ktoré má nejakú veľkosť (asi 3x3).

Vždy, keď váš had vytvorí slučku so svojím územím, sa pridá vnútro slučky
k vášmu územiu. Pozor --- telo vášho hada sa neráta ako vaše územie.
Ak do neho narazí hocičo (hlava nejakého hada, aj vašeho), tak váš had
zomrie. Územie vám ale zostane... kým vám ho ostatní hadi nezoberú...

Ako telo vášho hada sa ráta to, čo trčí von z vášho územia. Dôsledkom
toho je, že vo vašom území do vášho tela nevie naraziť žiaden iný had
(lebo žiadne telo nemáte, máte len hlavu).

Takisto nechcete, aby váš had nabúral do stien alebo do okraju mapy.

Na mape sa taktiež nachádzajú dva rôzne bonusy. Prvým z nich je
dvojnásobná rýchlosť na určitú dobu. Druhý spôsobí, že telo vášho
hada sa okamžite zmení na jeho územie. V momente, keď bonus spapáte,
ho aj hneď použijete. Bonusy sa objavuju na specialnych polickach,
v kazdom kole s urcitou pravdepodobnostou.

Ako sa ťahá
-----------

V každom kole dostanete kompletný stav hry --- teda ako vyzerá celá
mapa, kde je hlava ktorého hráča, a podobné. Jediné, čo môžete urobiť
je rozhodnúť, do ktorého smeru natočíte svoju hlavu (ako snake).
Celé telo sa potom pohne za ňou.

Dávajte si pozor, aby ste sa neotočili o 180 stupňov, keď sa nenachádzate
vo svojom území --- vtedy pravdepodobne narazíte do svojho vlastného tela.

Pravidlá hry
------------

Pravidlá sú veľmi jednoduché, ako bolo z časti popísané vyššie. Pre
konkrétne informácie odporúčam plakať, alebo nahliadnuť do update.cpp
a poloviť v komentároch.

Mapy
----

