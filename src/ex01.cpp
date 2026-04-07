#include <string>
#include <iostream>
#include <limits>

size_t folding_string_hash(std::string const& s, size_t max) {
    size_t hash = 0;

    for (char const& c : s) {
        hash += c;
    }

    hash%=max;
    return hash;
};

size_t folding_string_ordered_hash(std::string const& s, size_t max) {
    size_t hash = 0;

    const int n = s.size();
    for (int i = 0; i < n; i++) {
        hash+= s[i] * (i+1);
        hash%=max;
    }

    return hash;
};

size_t polynomial_rolling_hash(std::string const& s, size_t p, size_t m) {
    size_t hash = 0;
    size_t power = 1;

    const int n = s.size();
    for (char const& c : s) {
        hash+= c * power;
        power*=p;
        hash%=m;
    }

    return hash;
};

int main() {
    std::string secret = "Ma phrase secrète";
    int max = std::numeric_limits<int>::max();

    std::cout << folding_string_hash(secret,max) << "\n";
    std::cout << folding_string_ordered_hash(secret,max) << "\n";
    std::cout << polynomial_rolling_hash(secret,31,1000000009);

    return 0;
}