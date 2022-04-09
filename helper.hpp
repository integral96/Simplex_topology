#ifndef HELPER_HPP
#define HELPER_HPP

#include <type_traits>
#include <stdio.h>

namespace my {
template<class T, int N>
    struct helper
    {
        static constexpr T pow(const T x){
            return helper<T, N-1>::pow(x) * x;
        }
    };

    template<class T>
    struct helper<T, 1>
    {
        static constexpr T pow(const T x){
            return x;
        }
    };

    template<class T>
    struct helper<T, 0>
    {
        static constexpr T pow(const T x){
            return T(1);
        }
    };
    template<int N, class T>
    T constexpr pow(T const x)
    {
        return helper<T, N>::pow(x);
    }

    template <size_t N, size_t I, class Closure>
    typename std::enable_if_t<(I == N)> is_meta_loop(Closure&) {}

    template <size_t N, size_t I, class Closure>
    typename std::enable_if_t<(I < N)> is_meta_loop(Closure& closure) {
        closure.template apply<I>();
        is_meta_loop<N, I + 1>(closure);
    }
    template <size_t N, class Closure>
    void meta_loop(Closure& closure) {
        is_meta_loop<N, 0>(closure);
    }
}

#endif // HELPER_HPP
