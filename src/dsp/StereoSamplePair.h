
#pragma once
#include <complex>

struct StereoPair
{
    float left, right;

    StereoPair(float leftSample, float rightSample)
        : left(leftSample)
        , right(rightSample)
    {
    }
    StereoPair()
        : StereoPair(0.f, 0.f)
    {
    }
    StereoPair(const StereoPair&) = default;
    StereoPair& operator=(const StereoPair&) = default;
    StereoPair(StereoPair&& other) = default;
    StereoPair& operator=(StereoPair&& other) = default;

    StereoPair& operator*(const float gain)
    {
        left *= gain;
        right *= gain;
        return *this;
    }
    
    //
    //    StereoPair& operator+(StereoPair& other)
    //    {
    //        left += other.left;
    //        right += other.right;
    //        return *this;
    //    }
};

template <class T>
StereoPair operator*(StereoPair& lhs, const T gain)
{
    return StereoPair{lhs.left * gain, lhs.right * gain};
}

template <class T>
StereoPair operator*(const T gain, StereoPair& rhs)
{
    return StereoPair{rhs.left * gain, rhs.right * gain};
}

//StereoPair operator+(StereoPair& lhs, StereoPair& other)
//{
//    return StereoPair{lhs.left + other.left, lhs.right + other.right};
//}
