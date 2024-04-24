#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <utility>

namespace baihua {

    template<class T1, class T2, int (*CmpT1)(const T1 &, const T1 &), int (*CmpT2)(const T2 &, const T2 &)>
    class pair {
    public:
        T1 first;
        T2 second;

        constexpr pair() : first(), second() {}

        pair(const pair &other) = default;

        pair(pair &&other) = default;

        pair(const T1 &x, const T2 &y) : first(x), second(y) {}

        pair &operator=(const pair &other) {
            if (this == &other) return *this;
            first = other.first;
            second = other.second;
            return *this;
        }

    };

    template<class T1, class T2, int (*CmpT1)(const T1 &, const T1 &), int (*CmpT2)(const T2 &, const T2 &)>
    int CmpPair(const pair<T1, T2, CmpT1, CmpT2> &x, const pair<T1, T2, CmpT1, CmpT2> &y) {
        int result = CmpT1(x.first, y.first);
        if (result == 0) return CmpT2(x.second, y.second);
        else return result;
    }

}

#endif
