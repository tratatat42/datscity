#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <tuple>
#include <algorithm>

using namespace std;

namespace aux{
  template<std::size_t...> struct seq{};

  template<std::size_t N, std::size_t... Is>
    struct gen_seq : gen_seq<N-1, N-1, Is...>{};

  template<std::size_t... Is>
    struct gen_seq<0, Is...> : seq<Is...>{};

  template<class Ch, class Tr, class Tuple, std::size_t... Is>
    void print_tuple(std::basic_ostream<Ch,Tr>& os, Tuple const& t, seq<Is...>){
      using swallow = int[];
      (void)swallow{0, (void(os << (Is == 0? "" : ", ") << std::get<Is>(t)), 0)...};
    }
} // aux::

template<class Ch, class Tr, class... Args>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t) -> std::basic_ostream<Ch, Tr>&
{
  os << "<";
  aux::print_tuple(os, t, aux::gen_seq<sizeof...(Args)>());
  return os << ">";
}

template<typename U, typename V>
ostream &operator<<(ostream &s, const pair<U, V> &x) {
    s << "(" << x.first << ", " << x.second << ")";
    return s;
}

template<typename U>
ostream &operator<<(ostream &s, const vector<U> &x) {
    s << "[";
    bool was = false;
    for (const auto& it : x) {
        if (was) {
            s << ", ";
        }
        was = true;
        s << it;
    }
    s << "]";
    return s;
}

template<typename U>
ostream &operator<<(ostream &s, const set<U> &x) {
    s << "{";
    bool was = false;
    for (const auto& it : x) {
        if (was) {
            s << ", ";
        }
        was = true;
        s << it;
    }
    s << "}";
    return s;
}

template<typename U>
ostream &operator<<(ostream &s, const multiset<U> &x) {
    s << "{{";
    bool was = false;
    for (const auto& it : x) {
        if (was) {
            s << ", ";
        }
        was = true;
        s << it;
    }
    s << "}}";
    return s;
}

template<typename U, typename V>
ostream &operator<<(ostream &s, const map<U, V> &x) {
    s << "{";
    bool was = false;
    for (const auto& it : x) {
        if (was) {
            s << ", ";
        }
        was = true;
        s << it.first << ": " << it.second;
    }
    s << "}";
    return s;
}


string trim(const string &s) {
    const string WHITESPACE = " \n\r\t\f\v";
    string ret = s;
    size_t start = ret.find_first_not_of(WHITESPACE);
    ret = (start == string::npos) ? "" : ret.substr(start);
    size_t end = ret.find_last_not_of(WHITESPACE);
    ret = (end == string::npos) ? "" : ret.substr(0, end + 1);
    return ret;
}


vector<string> comma_splitter(const string& value) {
    vector<string> ret;
    string last;
    int balance = 0;
    for (const auto& c : (value + ",")) {
        if (c == '(' || c == '[') {
            balance += 1;
        } else if (c == ')' || c == ']') {
            balance -= 1;
        } if (balance == 0 && c == ',') {
            if (last.size()) {
                ret.push_back(trim(last));
            }
            last = "";
        } else {
            last += c;
        }
    }
    return ret;
}

template <typename Head, typename... Tail>
void debug_out(size_t idx, const vector<string>& args, const Head& H) {
    cerr << "`" << args[idx] << "` = " << H << endl;
}

template <typename Head, typename... Tail>
void debug_out(size_t idx, const vector<string>& args, const Head& H, const Tail&... T) {
    cerr << "`" << args[idx] << "` = " << H;
    if (idx + 1 < args.size()) {
        cerr << ", ";
    }
    debug_out(idx + 1, args, T...);
}

template <typename Head, typename... Tail>
void debug_out(int line, const string& args, const Head& H, const Tail&... T) {
    cerr << "#" << line << ": ";
    debug_out(0, comma_splitter(args), H, T...);
}

template <typename Head, typename... Tail>
void debug_out_inline(const Head& H) {
    cerr << H;
}

template <typename Head, typename... Tail>
void debug_out_inline(const Head& H, const Tail&... T) {
    cerr << H << ", ";
    debug_out_inline(T...);
}
