
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

Kľudne si prečítajte aj ostatné zdrojáky, ja, Tomi a Bui sa len potešíme, 
ale pri kódení vášho klienta vám asi nepomôžu.

Ako kódiť klienta
-----------------

Skopírujte obsah `klienti/template` do iného adresára a niečo v ňom nakóďte.

V koreni proboju spustite `make`, čím všetko skompilujete. (Ak váš klient nie je
vnútri `klienti`, nastavte v jeho `Makefile` správny `SERVERDIR` a spustite
`make` aj v ňom.)

Potom spustite `./server/server zaznamy/01 mapy/simple20x20-4.ppm klienti/vasklient
klienti/vasklient klienti/hlupy` To spustí hru s troma hráčmi (vaším, druhým
vaším a hlúpym) a uloží záznam do `zaznamy/01`. Ten si môžete pozrieť tak,
že najprv zmeníte working directory na `observer` (príkaz `cd observer`)
a potom zadáte príkaz `java -jar observer.jar ../zaznamy/01`.
Ak vám java nefunguje, môžete použiť alternatívny observer ktorý nieje taký 
pekný, a nezobrazuje všetko, spustíte ho `./observer ../zaznamy/01/observation`. 

Na začiatku hry dostane váš klient informácie o hre --- terén mapy.
Tiež má pritom viac času, aby sa mohol inicializovať --- server chvíľu počká, 
kým začne simulovať.

Server posiela klientovi každé kolo nový stav hry a čaká na jeho odpoveď. Časový
limit je dosť veľký, ale ak to bude naozaj dlho trvať, server nečaká. Ak klient
dlho neodpovedá, alebo program skončí (napríklad chybou), server ho znovu spustí
a pošle mu úvodné dáta (mapu).

Keď server spustíte u vás, je to len na skúšku. Na hlavnom počítači to beží na
ostro. Je tam aj webové rozhranie, cez ktoré môžete uploadovať vašich klientov.
Uploadujú sa zdrojáky a tie sa potom skompilujú (konkrétne sa spustí `make
naserveri SERVERDIR=/adresar/kde/je/server`).


Aký je proboj
-------------

Hra sa volá Mad Scientists.

Na mape sú labáky, mestá, skaly a voľné políčka.
Na začiatku hry vlastníte jeden labák, v ktorom môžete vyrábať robotov. Sila 
robora závisí od toho koľko éteru na jeho výrobu použijete --- čím viac tým 
silnejší. Éter získavate za keždý obsadený labák, mesto a za každých 9 políčok.
Políčo vlastní ten, koho robot na ňom bol ako posledný. Na každom políčku môže 
byť len jeden robot.

Vaším cielom je ovládnuť svet. Problém je, že nie ste jediní kto sa o to snaží.
Na mape sú labáky ostatných hráčov, ktorí tiež stavajú robotov. Kaď sa dvaja 
roboti stretnú, silnejší vyhráva a slabší zomiera.

Finálne skóre je množstvo éteru získané za celú hru.

Ako sa ťahá
-----------

V každom kole dostanete pohľad na mapu, tak ako ju vidieť z vášho územia.
To znamená celé vaše územie, a dve políčka okolo. O každom políčku ktoré vidíte,
viete kto ho vlastní, a aký silný robot tam stojí. Na ostatných políčkach je 
majiteľ `-1` a robot so silou `0`.

Váš ťah je postupnosť príkazov pre jednotlivích robotov, a pre labáky.
Každému robotovi môžete (nemusíte) povedať smer ktorým sa má pohnúť a každému 
labáku silu robota ktorého má postaviť. Ak robot alebo labák nedostane príkaz, 
nič nerobí.

Template klienta je v `C++`, ak chcete použiť iný jazyk, tu budú technické 
podrobnosti (ak bude mať niekto záujem):

Pravidlá hry
------------

V každom kole sa dejú jednotlivé udalosti v tomto poradí:
zomrú roboti ktorí išli proti múru,
pobijú sa roboti ktorí išli proti sebe (silnejší prejde, slabší zomrie),
postavia sa roboti v labákoch (ak je tam miesto),
pobijú sa roboti ktorí sú na jednom políčku (ak je viac rovnako silných, vyberie sa náhodne).

Pravidlá sú veľmi jednoduché, ako bolo z časti popísané vyššie. Pre
konkrétne informácie odporúčam pýtať sa, alebo nahliadnuť do update.cpp
a poloviť v komentároch --- haha komentáre tam niesu.

Mapy
----

Každá mapa má v názve rozmeri a počet hráčov ktorí sa na ňu zmestia.
Každá mapa je ohraničená skalami (observer niektoré skali zobrazuje ako vodu, pre vás v tom nieje rozdiel)
