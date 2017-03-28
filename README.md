# proboj-2017-jar - Mad Scientists

## Co treba na rozbehanie proboja?

Odporucame rozbehavat proboj na linuxe. Na windowse je to bolestive
a nie zdokumentovane, ale ak to date tak vas to urcite posilni...

Na rozbehanie grafickeho rozhrania na pozeranie zapasov ("observer")
je potrebna java 8.

`sudo apt-get install oracle-java8-installer`

Na niektore scripty je potrebny python2, a kadejake balicky v nom
(flask, ...)

## Ako rozbehat proboj?

O tom, ako rozbehat server a observer je napisane v subore
`dokumentacia.md`. (Pristupny aj cez web, ked je rozbehany webserver.)
Dalej popiseme, ako rozbehat webserver (potrebny na submitovanie).

Spustit webserver si viete pomocou bash-scriptu `./runweb` ktory
spustite z korena proboja. (Teda v `./proboj-2017-jar/` spustite
`./runweb`.)

Ked vam uz bezi web, tak ho viete navstivit na `0.0.0.0:5000`

Ak sa chcete zaregistrovat (co je nutne na to, aby ste mohli
submitovat botov), vojdite do priecinku `registracia` a spustite
bash-script `./register`. Dalej postupujte podla jeho pokynov.

## Ako urobit, aby sa zapasy sami generovali a zobrazovali?

Vojdete do priecinka `hlavnykomp` a v nom sa nachadzaju skripty

*   `pustaj-plamen`, ktory automaticky skompiluje vase submity, ked
ich odoslete cez webserver. Takisto do terminalu vypisuje pripadne
chybove hlasky pri kompilovani.

*   `pustaj-server` --- automaticke generovanie zapasov.

*   `pustaj-observer` --- automaticky zobrazuje zapasy.

## A co, ked nieco nefunguje?

Tak to treba opravit. Konkretne pokyny bohuzial neexistuju,
takze si asi trochu poplacete, ale co vas nezabije, to vas posilni.

<!--## O vyhodnoteni

Najprv bol "group stage", kde boli 2 skupiny. Na kazdej mape sa odohral
1 zapas, v ktorom boli vsetky timy skupiny. Z kazdej skupiny postupili
do finale 2 najlepsie umiestnene timy. Finale sa vyhodnotilo podobne
-- na kazdej mape sa odohral 1 zapas, v ktorom bojovali vsetky timy,
co sa dostali do finale.

Zapasy na vyhodnoteni si viete pozriet nasledovne:

*   Skopirujete priecinok zapasy1, zapasy2 alebo zapasy3 do priecinku
zaznamy. (`zapasy1` je prva skupina, `zapasy2` je druha a `zapasy3`
je finale)

*   Spustite z korena proboja bash-skript `./pustaj-observer2`.

Ak si chcete spravit vlastne vyhodnotenie, tak sa vam zidu skripty
`testall`, `testall1`, ... nachadzajuce sa v koreni proboja.
Konkretnejsie pokyny zatial neexistuju.

## Nejake obrazky na zaver

![](proboj3a.png)

![](proboj3b.png)

![](proboj3c.png)-->
