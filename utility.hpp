#pragma once

#include <cmath>
#include <vector>
#include <algorithm>

template<typename T>
struct Vector
{
    T x;
    T y;

    Vector() = default;
    Vector(const Vector<T>&) = default;
    Vector(const T x, const T y)
    {
        this->x = x;
        this->y = y;
    }

    template<typename T2>
    Vector(const Vector<T2>& other)
    {
        x = other.x;
        y = other.y;
    }

    Vector<T> operator-(const Vector<T>& other) const
    {
        return {x - other.x, y - other.y};
    }

    Vector<T> operator+(const Vector<T>& other) const
    {
        return {x + other.x, y + other.y};
    }
};

using Vectori = Vector<int>;
using Vectorf = Vector<float>;

float dist(Vectorf v1, Vectorf v2)
{
    Vectorf v(v1.x-v2.x, v1.y-v2.y);
    return std::sqrt(v.x*v.x + v.y*v.y);
}

template< typename T >
typename std::vector<T>::iterator
   insert_sorted( std::vector<T> & vec, T const& item )
{
    return vec.insert
        (
            std::upper_bound( vec.begin(), vec.end(), item ),
            item
        );
}

template< typename T, typename Pred >
typename std::vector<T>::iterator
    insert_sorted( std::vector<T> & vec, T const& item, Pred pred )
{
    return vec.insert
        (
           std::upper_bound( vec.begin(), vec.end(), item, pred ),
           item
        );
}
