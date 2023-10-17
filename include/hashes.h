#ifndef HASHES_
#define HASHES_
#include <mod_math.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>

template<size_t Pow, size_t mod = MOD>
struct Hash : public iMod<size_t, mod> {
    Hash(): iMod<size_t, mod>(0) {}
    template<typename T>
    Hash(const T& v): iMod<size_t, mod>(v) {}
    consteval static size_t P() {
        return Pow;
    }
    Hash<Pow, mod> operator<<(size_t i) {
        return (*this) * BinPow<size_t, mod>(Pow, i);
    }
};

template<size_t Pow, size_t Mod1, size_t Mod2>
struct DoubleHash {
    DoubleHash<Pow, Mod1, Mod2>& operator+=(const DoubleHash<Pow, Mod1, Mod2>& b) {
        h1 += b.h1;
        h2 += b.h2;
        return *this;
    }

    DoubleHash<Pow, Mod1, Mod2> operator+(const DoubleHash<Pow, Mod1, Mod2>& b) {
        DoubleHash<Pow, Mod1, Mod2> a(*this);
        a += b;
        return a;
    }

    DoubleHash<Pow, Mod1, Mod2>& operator*=(size_t b) {
        h1 *= b;
        h2 *= b;
        return *this;
    }

    DoubleHash<Pow, Mod1, Mod2> operator*(const DoubleHash<Pow, Mod1, Mod2>& b) {
        DoubleHash<Pow, Mod1, Mod2> a(*this);
        a *= b;
        return a;
    }

    DoubleHash<Pow, Mod1, Mod2>& operator-=(const DoubleHash<Pow, Mod1, Mod2>& b) {
        h1 -= b.h1;
        h2 -= b.h2;
        return *this;
    }

    DoubleHash<Pow, Mod1, Mod2> operator-(const DoubleHash<Pow, Mod1, Mod2>& b) {
        DoubleHash<Pow, Mod1, Mod2> a(*this);
        a -= b;
        return a;
    }

    DoubleHash<Pow, Mod1, Mod2>& operator<<=(size_t i) {
        h1 <<= i;
        h2 <<= i;
        return *this;
    }

    DoubleHash<Pow, Mod1, Mod2> operator<<(size_t i) {
        DoubleHash<Pow, Mod1, Mod2> a(*this);
        a <<= i;
        return a;
    }

    Hash<Pow, Mod1> h1, h2;
};

template<typename T, typename THash = Hash<257>>
struct HashMapping {
    THash operator()(const T& val) const {
        return THash(val);
    }
};

template<typename THash = Hash<257>>
struct HashMerger {
    THash operator()(const THash& l, const THash& r, const std::pair<size_t, size_t>& lseg, const std::pair<size_t, size_t>& rseg) const {
        return l << (rseg.second - lseg.first + 1) + r;
    }
};

template<typename container, typename THash = Hash<257>>
class HashedContainer {
public:
    HashedContainer(const container& c) : Hash_(c.size()), HashRev_(c.size()) {
        Hash_[0] = c[0];
        HashRev_.back() = c.back();
        for (size_t i = 1; i < c.size(); ++i) {
            Hash_[i] = Hash_[i - 1] * THash::P() + c[i];
        }
        for (int64_t i = int64_t(c.size()) - 2; i > -1; --i) {
            HashRev_[i] = HashRev_[i + 1] * THash::P() + c[i];
        }
    }
    THash Hash(size_t l, size_t r) {
        return Hash_[r] - ((l ? Hash_[l - 1] : THash()) << (r - l + 1));
    }
    THash HashReversed(size_t l, size_t r) {
        return HashRev_[l] - ((r + 1 < HashRev_.size() ? HashRev_[r + 1] : THash()) << (r - l + 1));
    }
    bool CheckPolyndrom(size_t l, size_t r) {
        return Hash(l, r) == HashReversed(l, r);
    }
    std::pair<size_t, size_t> LongestPoly() {
        std::pair<size_t, size_t> ans{};
        for (int64_t i = 0; i < Hash_.size(); ++i) {
            int64_t l = 0, r = Hash_.size() / 2 + 2;
            while (l < r - 1) {
                int64_t m = (l + r) / 2;
                if (i - m >= 0 && i + m < Hash_.size() && CheckPolyndrom(i - m, i + m)) {
                    l = m;
                }
                else if (i - m >= 0 && i + m - 1 < Hash_.size() && CheckPolyndrom(i - m, i + m - 1)) {
                    l = m;
                }
                else {
                    r = m;
                }
            }
            if (i - l >= 0 && i + l < Hash_.size() && CheckPolyndrom(i - l, i + l)) {
                if (ans.second - ans.first < 2 * l) {
                    ans = { i - l, i + l };
                }
            }
            else if (l && i - l >= 0 && i + l - 1 < Hash_.size() && CheckPolyndrom(i - l, i + l - 1)) {
                if (ans.second - ans.first < 2 * l - 1) {
                    ans = { i - l, i + l - 1 };
                }
            }
        }
        return ans;
    }
private:
    std::vector<THash> Hash_, HashRev_;
};

#endif