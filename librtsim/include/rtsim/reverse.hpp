#ifndef REVERSE_HPP
#define REVERSE_HPP

// FROM: https://stackoverflow.com/a/28139075

// -------------------------------------------------------------------
// --- Reversed iterable

template <typename T>
struct reversion_wrapper {
    T &iterable;
};

template <typename T>
auto begin(reversion_wrapper<T> w) {
    return std::rbegin(w.iterable);
}

template <typename T>
auto end(reversion_wrapper<T> w) {
    return std::rend(w.iterable);
}

template <typename T>
reversion_wrapper<T> reverse(T &&iterable) {
    return {iterable};
}

#endif // REVERSE_HPP
