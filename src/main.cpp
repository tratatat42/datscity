#include <bits/stdc++.h>
#include "third_party/httplib.h"
#include "third_party/json.hpp"
#include "third_party/pretty_print.hpp"

using namespace std;
using namespace filesystem;
using namespace this_thread;
using namespace chrono;
using namespace httplib;
using namespace nlohmann;

#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define reunique(v) v.resize(unique(v.begin(), v.end()) - v.begin())
#define sz(v) ((int)(v).size())

#define vec1d(x) vector<x>
#define vec2d(x) vector<vec1d(x)>
#define vec3d(x) vector<vec2d(x)>
#define vec4d(x) vector<vec3d(x)>

#define ivec1d(x, n, v) vec1d(x)(n, v)
#define ivec2d(x, n, m, v) vec2d(x)(n, ivec1d(x, m, v))
#define ivec3d(x, n, m, k, v) vec3d(x)(n, ivec2d(x, m, k, v))
#define ivec4d(x, n, m, k, l, v) vec4d(x)(n, ivec3d(x, m, k, l, v))

#define nl "\n"

#define dbg(...) debug_out(__LINE__, #__VA_ARGS__, __VA_ARGS__)

typedef long double ld;
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int, int> pii;

template <typename T> T sqr(T x) { return x * x; }
template <typename T> T abs(T x) { return x < 0? -x : x; }
template <typename T> T gcd(T a, T b) { return b? gcd(b, a % b) : a; }
template <typename T> bool chmin(T &x, const T& y) { if (x > y) { x = y; return true; } return false; }
template <typename T> bool chmax(T &x, const T& y) { if (x < y) { x = y; return true; } return false; }

auto random_address = [] { char *p = new char; return (uint64_t) p; };
mt19937 rng(steady_clock::now().time_since_epoch().count() * (random_address() | 1));
mt19937_64 rngll(steady_clock::now().time_since_epoch().count() * (random_address() | 1));


const string SERVER_HOST = "https://games-test.datsteam.dev";
Client client(SERVER_HOST);

string read_auth_token() {
    ifstream fin("auth_token.txt");
    if (!fin.is_open()) {
        cerr << "auth_token.txt not found" << endl;
        exit(1);
    }
    string token;
    fin >> token;
    return token;
}

void setup_client() {
    string token = read_auth_token();
    Headers headers = {
        {"Accept", "application/json"},
        {"Content-Type", "application/json"},
        {"X-Auth-Token", token},
    };
    client.set_default_headers(headers);
    client.set_follow_location(true);
}

int main(int /* argc */, char** /* argv */) {
    setup_client();
    return 0;
}
