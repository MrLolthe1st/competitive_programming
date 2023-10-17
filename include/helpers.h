#ifndef HELPERS_
#define HELPERS_

#include <algorithm>

template<typename T>
void reverse(T& v) {
    std::reverse(v.begin(), v.end());
}

template<typename T>
void sort(T& v) {
    std::sort(v.begin(), v.end());
}

template<typename T>
void rotate(T& v, size_t i) {
    std::sort(v.begin(), v.begin() + i, v.end());
}

#endif
