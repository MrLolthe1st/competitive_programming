#ifndef SEGTREE_H_
#define SEGTREE_H_
#include <helpers.h>
#include <stddef.h>
#include <utility>
#include <vector>
#include <optional>
#include <algorithm>

template<typename T>
struct MapIdenity {
    constexpr T operator()(const T& val) const {
        return val;
    }
};


template<typename Merge, typename T, typename Mapping=MapIdenity<T>, typename V=typename std::invoke_result<Mapping, const T&>::type>
class TSegTree {
    template<typename Fn>
    struct Merger {
        Merger(Fn&& f) : F_(std::move(f)) {}
        Merger(const Fn& f) : F_(f) {}
        Merger(Merger<Fn>&& f) : F_(std::move(f.F_)) {}
        constexpr V operator()(const V& a, const V& b) {
            return F_(a, b);
        }
    private:
        Fn F_;
    };

    constexpr bool SegmentExists(size_t i) {
        while (i < Power_) {
            i <<= 1;
        }
        return i - Power_ < Data_.size();
    }

    constexpr std::pair<size_t, size_t> SegmentRange(size_t i) {
        size_t leftest = i;
        while (leftest < Power_) {
            leftest <<= 1;
        }
        size_t rightest = i;
        while (rightest < Power_) {
            rightest <<= 1;
            ++rightest;
        }
        return {std::min(Data_.size() - 1, leftest - Power_), std::min(Data_.size() - 1, rightest - Power_)};
    }

    // assuming to do not merge empty segments
    constexpr V MergeInternal(size_t i) {
        if (2 * i + 1 >= Tree_.size() || !Tree_[2 * i + 1]) {
            return *Tree_[2 * i];
        }
        return Merge_(*Tree_[2 * i], *Tree_[2 * i + 1]);
    }

    void Init() {
        size_t size = Data_.size();
        Power_ = 1;
        while (Power_ < size) {
            Power_ <<= 1;
        }
        size_t totalLen = Power_ + size;
        // Since V or T may be isn't default constructable, build tree in right manner:
        // build it in bottom-down manner, but in reverse ordering
        auto reverseIndex = [&] (size_t i) {
            return totalLen - i - 1;
        };

        Tree_.reserve(totalLen);
        for (size_t i = 0; i < Data_.size(); ++i) {
            Tree_.emplace_back(Map_(Data_[Data_.size() - i - 1]));
        }

        auto mergeAt = [this, &reverseIndex, totalLen] (size_t i) -> std::optional<V> {
            if (!SegmentExists(i)) {
                return std::nullopt;
            }
            if (2 * i + 1 >= totalLen || !Tree_[reverseIndex(2 * i + 1)]) {
                return Tree_[reverseIndex(2 * i)];
            }
            return Merge_(*Tree_[reverseIndex(2 * i)],  *Tree_[reverseIndex(2 * i + 1)]);
        };

        for (size_t i = Power_ - 1; i > 0; --i) {
            Tree_.emplace_back(mergeAt(i));
        }

        Tree_.emplace_back(std::nullopt);
        // finally, reverse tree
        std::reverse(Tree_.begin(), Tree_.end());
    }
public:
    TSegTree(Merge&& m, const std::vector<T>& v, Mapping&& map=MapIdenity<V>()) : Merge_(std::move(m)), Map_(std::move(map)), Data_(v.begin(), v.end()) {
        Init();
    }
    TSegTree(Merge&& m, const std::vector<T>& v, const Mapping& map) : Merge_(std::move(m)), Map_(map), Data_(v.begin(), v.end()) {
        Init();
    }
    TSegTree(const Merge& m, const std::vector<T>& v, Mapping&& map=MapIdenity<V>()) : Merge_(m), Map_(std::move(map)), Data_(v.begin(), v.end()) {
        Init();
    }
    TSegTree(const Merge& m, const std::vector<T>& v, const Mapping& map) : Merge_(m), Map_(map), Data_(v.begin(), v.end()) {
        Init();
    }

    constexpr void UpdateAt(size_t i, const T& val) {
        Data_[i] = val;
        i += Power_;
        Tree_[i] = Map_(val);
        do {
            i >>= 1;
            Tree_[i] = MergeInternal(i);
        } while (i > 1);
    }

    constexpr const T& GetAt(size_t i) const {
        return Data_[i];
    }

    template<typename Fn, typename Fn2>
    typename std::invoke_result<Fn2, const V&>::type Query(size_t l, size_t r, const Fn& merge, const Fn2& getAns) const {
        static_assert(std::is_same_v<typename std::invoke_result<Fn2, const V&>::type, typename std::invoke_result<Fn, const V&, const V&>::type>, "merge and getAns must return the same type");
        using RT = typename std::invoke_result<Fn2, V>::type;
        std::optional<RT> lRes;
        std::optional<RT> rRes;
        l += Power_;
        r += Power_;
        while (l < r) {
            if (l & 1) { // left of seg above
                if (lRes) {
                    lRes = merge(*lRes, getAns(*Tree_[l]));
                } else {
                    lRes = getAns(*Tree_[l]);
                }
            }
            if (!(r & 1)) { // right of seg above
                if (rRes) {
                    rRes = merge(getAns(*Tree_[r]), *rRes);
                } else {
                    rRes = getAns(*Tree_[r]);
                }
            }
            l = (l + 1) >> 1;
            r = (r - 1) >> 1;
        }

        if (l == r) {
            if (lRes) {
                lRes = merge(*lRes, getAns(*Tree_[l]));
            } else {
                lRes = getAns(*Tree_[l]);
            }
        }
        if (!rRes) {
            return *lRes;
        }
        if (!lRes) {
            return *rRes;
        }
        return merge(*lRes, *rRes);
    }
    
    template<typename Fn, typename Fn2>
    typename std::invoke_result<Fn2, const V&, std::pair<size_t, size_t>>::type QueryWithIndices(size_t l, size_t r, const Fn& merge, const Fn2& getAns) const {
        static_assert(std::is_same_v<std::invoke_result<Fn2, const V&, std::pair<size_t, size_t>>::type, std::invoke_result<Fn, const V&, const V&, std::pair<size_t, size_t>, std::pair<size_t, size_t>>::type>, "merge and getAns must return the same type");
        using RT = typename std::invoke_result<Fn2, const V&, std::pair<size_t, size_t>>::type;
        std::optional<RT> lRes;
        std::optional<RT> rRes;
        std::pair leftRange = {l, l};
        std::pair rightRange = {r, r};
        l += Power_;
        r += Power_;
        // TODO(): do not calculate SegmentRange()
        while (l < r) {
            if (l & 1) { // left of seg above
                auto range = SegmentRange(l);
                if (lRes) {
                    lRes = merge(*lRes, getAns(*Tree_[l], range), leftRange, range);
                } else {
                    lRes = getAns(Tree_[l], range);
                }
                leftRange.second = range.second;
            }
            if (!(r & 1)) { // right of seg above
                auto range = SegmentRange(r);
                if (rRes) {
                    rRes = merge(getAns(*Tree_[r], range), *rRes, range, rightRange);
                } else {
                    rRes = getAns(*Tree_[r], range);
                }
                rightRange.first = range.first;
            }
            l = (l + 1) >> 1;
            r = (r - 1) >> 1;
        }

        if (l == r) {
            auto range = SegmentRange(l);
            if (lRes) {
                lRes = merge(*lRes, getAns(*Tree_[l], range), leftRange, range);
            } else {
                lRes = getAns(*Tree_[l], range);
            }
            leftRange.second = range.second;
        }
        if (!rRes) {
            return *lRes;
        }
        if (!lRes) {
            return *rRes;
        }
        return merge(*lRes, *rRes, leftRange, rightRange);
    }
private:
    Merger<Merge> Merge_;
    Mapping Map_;
    std::vector<T> Data_;
    std::vector<std::optional<V>> Tree_;
    size_t Power_;
};
#endif
