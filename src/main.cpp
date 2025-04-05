#include <bits/stdc++.h>
#include "third_party/httplib.h"
#include "third_party/json.hpp"
#include "third_party/pretty_print.hpp"
#include "third_party/argparse.hpp"

using namespace std;
using namespace filesystem;
using namespace this_thread;
using namespace chrono;
using namespace httplib;
using namespace nlohmann;
using namespace argparse;

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

mt19937 rng(42);
mt19937_64 rngll(42);


const string SERVER_HOST = "https://games-test.datsteam.dev";
Client client(SERVER_HOST);

string read_auth_token(const string& file_path = "auth_token.txt") {
    ifstream fin(file_path);
    if (!fin) {
        throw runtime_error("Failed to open " + file_path);
    }
    string token;
    fin >> token;
    if (token.empty()) {
        throw runtime_error("Token is empty in " + file_path);
    }
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

void sleep_wait(system_clock::time_point next) {
    for (;;) {
        auto now = system_clock::now();
        auto delay = duration_cast<milliseconds>(next - now);
        cerr << "\r" << delay;
        delay = min(delay, milliseconds(100));
        if (delay.count() <= 0) {
            break;
        }
        sleep_for(delay);
    }
    cerr << nl;
}

string wait_next_round() {
    /*
    {
      "eventId": "1",
      "now": "2021-01-01T00:00:00Z",
      "rounds": [
        {
          "duration": 60,
          "endAt": "2021-01-01T00:00:00Z",
          "name": "Round 1",
          "repeat": 1,
          "startAt": "2021-01-01T00:00:00Z",
          "status": "active"
        }
      ]
    }
    */
    auto rounds_response = client.Get("/api/rounds");
    auto rounds = json::parse(rounds_response->body);
    auto now = system_clock::now();
    bool found = false;
    auto nearest_start = now;
    string round_name = "unknown";
    for (auto& round : rounds["rounds"]) {
        // parse start_at from %Y-%m-%dT%H:%M:%SZ to time
        tm tm = {};
        strptime(string(round["startAt"]).c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm);
        auto start_clock = system_clock::from_time_t(timegm(&tm));
        tm = {};
        strptime(string(round["endAt"]).c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm);
        auto end_clock = system_clock::from_time_t(timegm(&tm));
        if (end_clock < now) {
            continue;
        }
        if (!found || start_clock < nearest_start) {
            found = true;
            nearest_start = start_clock;
            round_name = round["name"];
        }
    }
    if (found) {
        cerr << "Waiting for next round: " << nearest_start << nl;
        sleep_wait(nearest_start);
    }
    return round_name;
}

/*
words.dir – направление
1 = [0, 0, -1]
2 = [1, 0, 0]
3 = [0, 1, 0]
*/

/*
UP(z)
^
|  FRONT(y)
| /
|/
o-----> RIGHT(x)
*/

enum Direction {
    UP = 0,
    RIGHT = 1,
    FRONT = 2,
};

bool same_type(const Direction& a, const Direction& b) {
    return (a == UP) == (b == UP);
}

struct Dir {
    int dx, dy, dz;

    bool is_simple_parallel(const Dir& other) const {
        return (dx && other.dx) || (dy && other.dy) || (dz && other.dz);
    }
};

ostream& operator<<(ostream& os, const Dir& d) {
    return os << "<" << d.dx << ";" << d.dy << ";" << d.dz << ">";
}

bool operator==(const Dir& a, const Dir& b) {
    return a.dx == b.dx && a.dy == b.dy && a.dz == b.dz;
}

Dir operator*(const Dir& d, int k) {
    return {d.dx * k, d.dy * k, d.dz * k};
}

Dir operator-(const Dir& d) {
    return {-d.dx, -d.dy, -d.dz};
}

struct Pos {
    int x, y, z;

    int z_main_diag() const {
        return abs(x) + abs(y);
    }

    int z_second_diag() const {
        return abs(x) - abs(y);
    }
};

ostream& operator<<(ostream& os, const Pos& p) {
    return os << "[" << p.x << ";" << p.y << ";" << p.z << "]";
}

Pos operator-(const Pos& p, const Dir& d) {
    return {p.x - d.dx, p.y - d.dy, p.z - d.dz};
}

Pos operator+(const Pos& p, const Dir& d) {
    return {p.x + d.dx, p.y + d.dy, p.z + d.dz};
}

const vector<Dir> DIRECTIONS = {
    {0, 0, -1},  // UP
    {1, 0, 0},  // RIGHT
    {0, 1, 0},  // FRONT
};

const vector<Dir> D6 = {
    {0, 0, 1},  // UP
    {1, 0, 0},  // RIGHT
    {0, 1, 0},  // FRONT
    {0, 0, -1}, // UP
    {-1, 0, 0}, // LEFT
    {0, -1, 0}, // BACK
};

struct Word {
    int id;
    Direction dir;
    Pos pos;
    int len;

    Pos start_position() const {
        return pos;
    }

    Pos end_position() const {
        return pos + DIRECTIONS[dir] * (len - 1);
    }
};

ostream& operator<<(ostream& os, const Word& w) {
    return os << "Word(" << w.id << ", " << w.dir << ", " << w.pos << ", " << w.len << ")";
}

typedef vector<int> iword;

const int UNDEF = -1;

struct Tower {
    int nx, ny, nz;
    vector<Word> words;
    set<int> used_words;
    vec3d(int) dir_grid;
    vec3d(int) grid;
    vec1d(int) n_words;
    int total_len;

    Tower() { }

    Tower(int nx, int ny, int nz) : nx(nx), ny(ny), nz(nz) {
        dir_grid = ivec3d(int, nx, ny, nz, 0);
        grid = ivec3d(int, nx, ny, nz, 0);
        n_words = ivec1d(int, nz, 0);
        total_len = 0;
    }
    
    bool inside(const Pos& pos) {
        return pos.x >= 0 && pos.x < nx && pos.y >= 0 && pos.y < ny && pos.z >= 0 && pos.z < nz;
    }

    bool can(int id, const iword& word, Direction dir, Pos pos, bool check_full_intersection = false) {
        if (id != UNDEF && used_words.count(id)) {
            return false;
        }
        int len = word.size();
        auto vec = DIRECTIONS[dir];
        if (!inside(pos + vec * (len - 1))) {
            return false;
        }
        if (!inside(pos)) {
            return false;
        }
        for (int i = 0; i < len; ++i) {
            Pos p = pos + vec * i;
            if (grid[p.x][p.y][p.z] && grid[p.x][p.y][p.z] != word[i]) {
                return false;
            }
            if (dir_grid[p.x][p.y][p.z] & (1 << dir)) {
                return false;
            }
            if (check_full_intersection && dir_grid[p.x][p.y][p.z] + (1 << dir) == 7) {
                return false;
            }
            if (!grid[p.x][p.y][p.z] && id != UNDEF) {
                for (auto& d : D6) {
                    if (i + 1 < len && d == vec) {
                        continue;
                    }
                    if (i > 0 && d == -vec) {
                        continue;
                    }
                    Pos e = p + d;
                    if (!inside(e)) {
                        continue;
                    }
                    if (grid[e.x][e.y][e.z]) {
                        return false;
                    }
                }
            }
        }
        return true;    
    }

    void add(int id, const iword& word, Direction dir, Pos pos) {
        used_words.insert(id);
        words.push_back({id, dir, pos, sz(word)});
        int len = word.size();
        for (int i = 0; i < len; ++i) {
            Pos p = pos + DIRECTIONS[dir] * i;
            grid[p.x][p.y][p.z] = word[i];
            dir_grid[p.x][p.y][p.z] |= 1 << dir;
        }
        total_len += len;

        if (dir != UP) {
            n_words[pos.z]++;
        }
    }

    Pos start_position() const {
        return words.back().start_position();
    }

    Pos end_position() const {
        return words.back().end_position();
    }

    double score() {
        auto minx = ivec1d(int, nz, INT_MAX);
        auto maxx = ivec1d(int, nz, 0);
        auto miny = ivec1d(int, nz, INT_MAX);
        auto maxy = ivec1d(int, nz, 0);
        auto letters = ivec1d(int, nz, 0);

        for (int x = 0; x < nx; x++)
            for (int y = 0; y < ny; y++)
                for (int z = 0; z < nz; z++)
                {
                    if (grid[x][y][z] != 0)
                    {
                        chmin(minx[z], x);
                        chmax(maxx[z], x);
                        chmin(miny[z], y);
                        chmax(maxy[z], y);
                        letters[z]++;
                    }
                }

        double sum = 0;
        for (int z = 0; z < nz; z++)
        {
            auto width = maxx[z] + 1 - minx[z];
            auto height = maxy[z] + 1 - miny[z];
            sum += (double)(z + 1) * letters[z] * (1 + n_words[z] / 4.0) * min(width, height) / max(width, height);
        }

        return sum;
    }

    void print(string filename) {
        ofstream fout(filename);
/*
        fout << "module Text(t)" << endl;
        fout << "{" << endl;
        fout << "    translate([0, 0, -0.05]) color([0, 0, 0, 0.5]) linear_extrude(0.1) resize([0.4, 0], auto = true) text(t, size = 0.5, halign = "center", valign = "center");" << endl;
        fout << "}" << endl;
        fout << endl;
        fout << "module Cube(t, c)" << endl;
        fout << "{
        fout << "    color(c) cube(0.9);" << endl;
        fout << endl;        
        fout << "    translate([0, 0.5, 0.5]) rotate([90, 0, -90]) Text(t);" << endl;
        fout << "    translate([0.9, 0.5, 0.5]) rotate([90, 0, 90]) Text(t);" << endl;
        fout << "    translate([0.5, 0, 0.5]) rotate([90, 0, 0]) Text(t);" << endl;
        fout << "    translate([0.5, 0.9, 0.5]) rotate([90, 0, 180]) Text(t);" << endl;
        fout << "    translate([0.5, 0.5, 0]) rotate([180, 0, 0]) Text(t);" << endl;
        fout << "    translate([0.5, 0.5, 0.9]) rotate([0, 0, 0]) Text(t);" << endl;
        fout << "}" << endl;
*/        
        for (int x = 0; x < nx; x++)
            for (int y = 0; y < ny; y++)
                for (int z = 0; z < nz; z++)
                    if (grid[x][y][z] != 0) {
                        auto color = dir_grid[x][y][z];
                        fout << "translate([" << x << ", " << y << ", " << z << "]) color([" << color % 2 << ", " << color / 2 % 2 << ", " << color / 4 % 2 << ", 0.7]) cube(0.9);" << endl;
//                        fout << "translate([" << x << ", " << y << ", " << z << "]) Cube(\"" << text << "\", [" << color % 2 << ", " << color / 2 % 2 << ", " << color / 4 % 2 << ", 0.7]);" << endl;
                    }
        fout.close();
    }
};

enum Stage {
    UP_STAGE,
    CORNER1_STAGE,
    CORNER2_STAGE,
    DOWN_STAGE,
    RANDOM_STAGE,
};

struct State {
    Tower tower;
    Stage stage;
};

typedef shared_ptr<State> StatePtr;


struct WeightedMove {
    StatePtr state;
    vector<int> score;
    Word word;
};

bool operator<(const WeightedMove& a, const WeightedMove& b) {
    if (a.state->stage != b.state->stage) {
        return a.state->stage > b.state->stage;
    }
    return a.score > b.score;
}

int main(int argc, char** argv) {
    ArgumentParser parser;
    parser.parse(argc, argv);
    
    string round_name = parser.get("round", "");
    string round_turn = parser.get("turn", "1");
    bool use_dump = !round_name.empty();
    
    setup_client();
    
    if (round_name.empty()) {
        round_name = wait_next_round();
    }
    dbg(round_name);

    auto round_folder = path("dumps") / round_name;
    if (!exists(round_folder)) {
        create_directories(round_folder);
    }

    auto dump_turn = [&](const json& words, const json& towers) {
        auto turn_folder = round_folder / to_string(words["turn"]);
        if (!exists(turn_folder)) {
            create_directories(turn_folder);
        }
        {
            auto words_file = turn_folder / "words.json";
            ofstream words_out(words_file);
            words_out << words.dump(2);
            words_out.close();
        }
        {
            auto towers_file = turn_folder / "towers.json";
            ofstream towers_out(towers_file);
            towers_out << towers.dump(2);
            towers_out.close();
        }
    };

    auto dump_build = [&](const json& words, const json& build) {
        auto turn_folder = round_folder / to_string(words["turn"]);
        if (!exists(turn_folder)) {
            create_directories(turn_folder);
        }
        {
            auto build_file = turn_folder / "build.json";
            dbg(build_file);
            ofstream build_out(build_file);
            build_out << build.dump(2);
            build_out.close();
            dbg("ok");
            return true;
        }
    };

    auto load_dump = [&](auto& words, auto& towers) {
        auto turn_folder = round_folder / round_turn;
        auto word_file = turn_folder / "words.json";
        auto towers_file = turn_folder / "towers.json";
        if (!exists(word_file) || !exists(towers_file)) {
            throw runtime_error("Dump files not found");
        }
        ifstream words_in(word_file);
        words = json::parse(words_in);
        ifstream towers_in(towers_file);
        towers = json::parse(towers_in);
    };

    for (;;) {
        auto words_data = json::object();
        auto towers_data = json::object();
        if (use_dump) {
            load_dump(words_data, towers_data);
        } else {
            auto words_response = client.Get("/api/words");
            words_data = json::parse(words_response->body);
            auto towers_response = client.Get("/api/towers");
            dbg(towers_response->body);
            towers_data = json::parse(towers_response->body);
            int next_turn_sec = words_data["nextTurnSec"];
            if (next_turn_sec < 10) {
                cerr << "Next turn in " << next_turn_sec << " seconds" << nl;
                sleep_wait(system_clock::now() + seconds(next_turn_sec + 1));
                continue;
            }
            dump_turn(words_data, towers_data); 
        }

        auto next_turn_time = system_clock::now() + seconds(words_data["nextTurnSec"]) + milliseconds(500);

        auto sizes = words_data["mapSize"];
        dbg(sizes);
        Tower tower(sizes[0], sizes[1], sizes[2]);

        vector<iword> words;
        for (auto& w : words_data["words"]) {
            auto word_utf = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(w);
            auto word = vector<int>(word_utf.begin(), word_utf.end());
            words.push_back(word);
        }

        auto tower_data = towers_data["tower"];
        auto tower_words = tower_data["words"];
        dbg(sz(tower_words));
        for (auto& tower_word : tower_words) {
            auto word_utf = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(tower_word["text"]);
            auto word = vector<int>(word_utf.begin(), word_utf.end());
            auto word_id = UNDEF;
            Direction dir = (Direction)((int)tower_word["dir"] - 1);
            Pos pos = {tower_word["pos"][0], tower_word["pos"][1], tower_word["pos"][2]};
            assert(tower.can(word_id, word, dir, pos));
            tower.add(word_id, word, dir, pos);
        }

        for (int id : words_data["usedIndexes"]) {
            tower.used_words.insert(id);
        }

        map<pair<int, int>, vector<int>> words_library;
        map<int, int> char_offset;
        for (int i = 0; i < words.size(); ++i) {
            if (tower.used_words.count(i)) {
                continue;
            }
            auto& word = words[i];
            if (sz(word) <= 2) {  // FIXME
                continue;
            }
            int len = word.size();
            for (int j = 0; j < word.size(); ++j) {
                auto& c = word[j];
                words_library[{c, j}].push_back(i);
                words_library[{c, -len + j}].push_back(i);
                chmax(char_offset[c], j);
                chmax(char_offset[c], abs(-len + j));
            }
        }
        vector<StatePtr> states;

        const int LIMIT = 10;
        if (sz(tower.words) == 0) {
            for (int i = 0; i < LIMIT; ++i) {
                auto id = rng() % words.size();
                if (tower.used_words.count(id)) {
                    continue;
                }
                auto word = words[id];
                auto dir = (Direction)(rng() % 2 + 1);
                StatePtr state = make_shared<State>();
                state->tower = tower;
                state->tower.add(id, word, dir, Pos{0, 0, 0});
                state->stage = UP_STAGE;
                states.push_back(state);
            }
        } else {
            StatePtr state = make_shared<State>();
            state->tower = tower;
            state->stage = UP_STAGE;
            states.push_back(state);
        }

        Tower target_tower = tower;
        bool found = false;
        int n_broken = 0;
        while (sz(states) && !found) {
            vector<WeightedMove> cands;
            dbg(sz(states));
            for (auto& state : states) {
                auto& tower = state->tower;
                auto& stage = state->stage;
                Word word;
                if (stage == RANDOM_STAGE) {
                    vector<int> indices;
                    int opt = -1;
                    for (int i = 0; i < tower.words.size(); ++i) {
                        auto word = tower.words[i];
                        if (word.dir != UP) {
                            continue;
                        }
                        Pos pos = word.end_position();
                        if (tower.dir_grid[pos.x][pos.y][pos.z] == 7) {
                            continue;
                        }
                        if (opt < pos.z) {
                            indices.clear();
                            opt = pos.z;
                        }
                        if (pos.z == opt) {
                            indices.push_back(i);
                        }
                    }
                    if (indices.empty()) {
                        continue;
                    }
                    int index = indices[rng() % indices.size()];
                    word = tower.words[index];
                } else {
                    word = tower.words.back();
                }

                bool found = false;
                for (auto dir : {UP, RIGHT, FRONT} ) {
                    if (same_type(word.dir, dir)) {
                        continue;
                    }
                    for (int index = 0; index < word.len; ++index) {
                        auto pos = word.pos + DIRECTIONS[word.dir] * index;
                        int chr = tower.grid[pos.x][pos.y][pos.z];
                        if (tower.dir_grid[pos.x][pos.y][pos.z] & (1 << dir)) {
                            continue;
                        }

                        for (int offset = -char_offset[chr]; offset <= char_offset[chr]; ++offset) {
                            auto k = make_pair(chr, offset);
                            auto it = words_library.find(k);
                            if (it == words_library.end()) {
                                continue;
                            }
                            for (auto id : it->second) {
                                int shift = offset < 0? sz(words[id]) + offset : offset;
                                auto start = pos - DIRECTIONS[dir] * shift;
                                if (tower.can(id, words[id], dir, start, true)) {
                                    int len = words[id].size();
                                    auto end = start + DIRECTIONS[dir] * (len - 1);
                                    int min_z = min(start.z, end.z);
                                    int max_z = max(start.z, end.z);
                                    vector<int> res;
                                    if (stage == UP_STAGE) {
                                        res = {max_z, -sz(words[id]), -start.z_main_diag()};
                                    } else if (stage == CORNER1_STAGE) {
                                        res = {min_z, -end.z_second_diag(), -sz(words[id])};
                                    } else if (stage == CORNER2_STAGE) {
                                        res = {min_z, end.z_main_diag(), -sz(words[id])};
                                    } else if (stage == DOWN_STAGE) {
                                        res = {-min_z, -sz(words[id]), start.z_main_diag()};
                                    }
                                    cands.emplace_back(state, res, Word{id, dir, start, len});
                                    found = true;
                                }
                            }
                        }
                    }
                }
                if (!found) {
                    string debug_file = "debug/broken_" + to_string(n_broken++) + "-" + to_string(tower.words.size()) + ".scad";
                    tower.print(debug_file);
                }
            }
            states.clear();
            sort(all(cands));

            set<int> seen_words;
            for (auto& cand : cands) {
                auto state = cand.state;
                auto tower = state->tower;
                auto stage = state->stage;
                
                if (seen_words.count(cand.word.id)) {
                    continue;
                }
                seen_words.insert(cand.word.id);

                tower.add(cand.word.id, words[cand.word.id], cand.word.dir, cand.word.pos);
                target_tower = tower;

                Pos start_pos = cand.word.start_position();
                Pos end_pos = cand.word.end_position();
                int min_z = min(start_pos.z, end_pos.z);
                int max_z = max(start_pos.z, end_pos.z);
                int max_x = max(start_pos.x, end_pos.x);
                int max_y = max(start_pos.y, end_pos.y);
                if (stage == UP_STAGE && max_z == tower.nz - 1) {
                    stage = CORNER1_STAGE;
                } else if (stage == CORNER1_STAGE && max_y == tower.ny - 1) {
                    stage = CORNER2_STAGE;
                } else if (stage == CORNER2_STAGE && max_x == tower.nx - 1) {
                    stage = DOWN_STAGE;
                } else if (stage == DOWN_STAGE && min_z == 0 && tower.words.back().dir != UP) {
                    target_tower = tower;
                    found = true;
                }

                auto new_state = make_shared<State>();
                new_state->tower = tower;
                new_state->stage = stage;
                states.push_back(new_state);

                if (sz(states) == LIMIT) {
                    break;
                }
            }
        }

        {
            target_tower.print("debug/target.scad");
        }
        dbg("Score", target_tower.score());

        auto build_words = json::array();
        for (auto& word : target_tower.words) {
            if (word.id == UNDEF) {
                continue;
            }
            auto build_word = json::object();
            build_word["id"] = word.id;
            build_word["dir"] = word.dir + 1;
            build_word["pos"] = json::array();
            build_word["pos"].push_back(word.pos.x);
            build_word["pos"].push_back(word.pos.y);
            build_word["pos"].push_back(word.pos.z);
            build_words.push_back(build_word);
        }
        auto build = json::object();
        build["done"] = true;
        build["words"] = build_words;

        if (use_dump) {
            break;
        }

        if (dump_build(words_data, build)) {
            auto build_response = client.Post("/api/build", build.dump(), "application/json");
            dbg(build_response->body);
        }
        sleep_wait(next_turn_time);
    }
    return 0;
}
