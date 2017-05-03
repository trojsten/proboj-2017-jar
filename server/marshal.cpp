//interface pre komunikáciu s klientami (universal proboj magic)
using namespace std;

#include "marshal.h"

template<> void uloz<int>(ostream& buf, const int& in) {
    buf << in << " ";
}

template<> void nacitaj<int>(istream& buf, int& out) {
    buf >> out;
}

template<> void uloz<bool>(ostream& buf, const bool& in) {
    buf << in << " ";
}

template<> void nacitaj<bool>(istream& buf, bool& out) {
    buf >> out;
}

// uloz() pre (niektore) enumy z common.h
#define reflectenum(T) template<> void uloz<T> (ostream& out, const T& in) { uloz(out,  (const int &)in);}
#include "common.h"
#undef reflectenum

// nacitaj() pre (niektore) enumy z common.h
#define reflectenum(T) template<> void nacitaj<T> (istream& in, T& out) { nacitaj(in, (int &)out); }
#include "common.h"
#undef reflectenum

// uloz() pre (niektore) struktury z common.h
#define reflection(T) template<> void uloz<T> (ostream& out, const T& in) {
#define member(x) uloz(out, in.x);
#define end() }
#include "common.h"
#undef reflection
#undef member
#undef end

// nacitaj() pre (niektore) struktury z common.h
#define reflection(T) template<> void nacitaj<T> (istream& in, T& out) {
#define member(x) nacitaj(in, out.x);
#define end() }
#include "common.h"
#undef reflection
#undef member
#undef end
