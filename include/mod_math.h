#ifndef MOD_MATH_
#define MOD_MATH_
#include <stdint.h>
#include <istream>
#include <ostream>
#include <optional>

constexpr size_t MOD = 1e9+7;
template<typename T, T mod = MOD>
constexpr T BinPow(T a, int64_t pow) {
    T r = 1;
    while (pow) {
        if (pow & 1) {
            r = (r * a) % mod;
        }
        a = (a * a) % mod;
        pow >>= 1;
    }
    return r;
}

template<typename T>
T GCDEx(T a, T b, T & x, T & y) {
    if (a == 0) {
        x = 0; y = 1;
        return b;
    }
    T x1, y1;
    T d = GCDEx(b%a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
	return d;
}

template<typename T>
std::optional<T> Inverse(T a, T m) {
    T x, y;
    T g = GCDEx(a, m, x, y);
    if (g != 1) {
        return std::nullopt;
    }
    return (x % m + m) % m;
}


template<typename T, T mod = MOD>
struct iMod {
    iMod() : Value_(0) {}
    template<typename V>
    iMod(const V& val) : Value_(val % mod) {}

    friend std::ostream& operator<<(std::ostream& os, const iMod<T, mod>& n) {
        return os << n.Value_;
    }

    friend std::istream& operator>>(std::istream& is, iMod<T, mod>& n) {
        return is >> n.Value_;
    }

    iMod<T, mod> operator+(const iMod<T, mod>& other) const {
        return (Value_ + other.Value_) % mod;
    }

    iMod<T, mod>& operator+=(const iMod<T, mod>& other) {
        Value_ = (Value_ + other.Value_) % mod;
        return *this;
    }

    iMod<T, mod> operator-(const iMod<T, mod>& other) const {
        return (mod + Value_ - other.Value_) % mod;
    }

    iMod<T, mod>& operator-=(const iMod<T, mod>& other) {
        Value_ = (mod + Value_ - other.Value_) % mod;
        return *this;
    }
    
    iMod<T, mod> operator*(const iMod<T, mod>& other) const {
        return (Value_ * other.Value_) % mod;
    }

    iMod<T, mod>& operator*=(const iMod<T, mod>& other) {
        Value_ = (Value_ * other.Value_) % mod;
        return *this;
    }

    iMod<T, mod> operator/(const iMod<T, mod>& other) const {
        auto res = Inverse(other.Value_, mod);
        return (Value_ * *res) % mod;
    }

    iMod<T, mod>& operator/=(const iMod<T, mod>& other) {
        auto res = Inverse(other.Value_, mod);
        Value_ = (Value_ * *res) % mod;
        return *this;
    }

    bool operator==(const iMod<T, mod>& other) const {
        return Value_ == other.Value_;
    }
private:
    T Value_;
};

#endif