#include <bits/stdc++.h>
using namespace std;

#define DBG(s) cerr << (s) << '\n';

#define ll long long
#define ull unsigned ll
#define ld long double
#define point complex<ll>
#define pll pair<ll, ll>
#define vll vector<ll>

#define INF 9223372036854775807
#define PI 3.141592653589793238
#define MOD 1000000009

#define FOR(i, low, high) for(ll i = (low); i < (ll)(high); ++i)
#define REP(i, n) for(ll i = 1; i <= (ll)(n); ++i)
#define ROF(i, n) for(ll i = (ll)((n)-1); i >= 0; --i)
#define MP(x, y) (make_pair((x), (y)))

void printv(vll &v) {
    FOR(i, 0, (signed ll)v.size() - 1)
        cout << v[i] << " ";
    cout << v.back() << "\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cout << point(2, 5) << endl;
}
