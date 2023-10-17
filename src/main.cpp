#define _CRT_SECURE_NO_WARNINGS

#include <helpers.h>
#include <mod_math.h>
#include <segtree.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <hashes.h>
#include <set>
#include <vector>
#include <string>

#ifdef PREPROCESSOR_
####
#endif

void Solve(std::istream& cin, std::ostream& cout) {
    int n, m;
    cin >> n >> m;
    std::vector<int> a(n), b(m);
    for (auto &e: a) cin >> e;
    for (auto &e: b) cin >> e;
    auto ah = HashedContainer(a), bh = HashedContainer(b);
    auto r = ah.LongestPoly();
    cout << r.second - r.first + 1 << "\n";
}

void SolveSlow(std::istream& cin, std::ostream& cout) {

}

bool Checker(std::istream& cin, std::istream& out1, std::istream& out2) {
    return true;
}

bool check(const std::string& input) {
    std::stringstream in1, in2, out1, out2, fin;
    in1 << input; in2 << input; fin << input;
    in1.flush(); in2.flush(); fin.flush();
    Solve(in1, out1);
    SolveSlow(in2, out2);
    return Checker(fin, out1, out2);
}

std::string GenerateInput() {
    return "";
}

void Stress() {
    while (1) {
        auto input = GenerateInput();
        if (!check(input)) {
            throw std::runtime_error(":(");
        }
    }
}

int main() {
    #ifndef LOCAL_
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    std::cout.tie(0);
    #else
    auto __beg = std::chrono::steady_clock::now();
    #endif

    int t = 1;
    //std::cin >> t;
    while (t--) {
        Solve(std::cin, std::cout);
        std::cout << "\n";
    }

    #ifdef LOCAL_
    std::cerr << "Completed in " << std::setprecision(7) << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - __beg).count() * 1e-6 << "s\n";
    #endif
}