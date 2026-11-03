#pragma once
#include <utility>

template <typename TFirst, typename TSecond>
struct Pair
{
    TFirst first;
    TSecond second;

    Pair() = default;
    Pair(const TFirst &f, const TSecond &s) : first(f), second(s) {}
    Pair(TFirst &&f, TSecond &&s) : first(std::move(f)), second(std::move(s)) {}
};