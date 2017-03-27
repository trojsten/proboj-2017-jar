//interface pre komunikáciu s klientami
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

template<> void uloz<direction>(std::ostream& out, const direction& in) {
    uloz(out, (const int &)in);
}

template<> void nacitaj<direction>(std::istream& in, direction& out) {
    nacitaj(in, (int &)out);
}

template<> void uloz<square_type>(std::ostream& out, const square_type& in) {
    uloz(out, (const int &)in);
}

template<> void nacitaj<square_type>(std::istream& in, square_type& out) {
    nacitaj(in, (int &)out);
}

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
