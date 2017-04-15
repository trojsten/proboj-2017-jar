//universal magic
#include <string>
using namespace std;

#include "klient.h"
#include "util.h"

#define CAS_DO_RESTARTU 1000

Klient::Klient () {}

Klient::Klient (string _meno, string _uvodneData, string adresar, string execCommand, string zaznamovyAdresar)
    : meno(_meno), uvodneData(_uvodneData), poslRestart(-1)
{
    vector<string> command;
    command.push_back(execCommand);
    proces.setProperties(command, adresar, zaznamovyAdresar + "/" + meno + ".log");
}

Klient::Klient (string _meno, string _uvodneData, string adresar, string zaznamovyAdresar)
    : Klient(_meno, _uvodneData, adresar, "./hrac", zaznamovyAdresar) {}

void Klient::restartuj () {
    long long cas = gettime();
    if (cas - poslRestart > CAS_DO_RESTARTU) {
        loguj("restartujem klienta %s", meno.c_str());
        precitane.clear();
        proces.restartuj();

        posli(uvodneData);
        poslRestart = cas;
    }
}

string Klient::citaj (unsigned cap) {
    string nove = proces.read(cap);
    int i = nove.size();
    while (i > 0 && nove[i-1] != '\n') {
        // vraciame iba hotovu odpoved, uzavretu znakom noveho riadku
        i--;
    }
    if (i == 0) {
        precitane += nove;
        return "";
    }
    string res = precitane + nove.substr(0,i);
    precitane = nove.substr(i);
//     loguj("%s\n",res.c_str());
    return res;
}

void Klient::posli (string data) {
    proces.write(data);
}

void Klient::zabi () {
    proces.zabi();
}

bool Klient::zije () {
    return proces.zije();
}
