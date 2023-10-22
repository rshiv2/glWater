#ifndef COMMON_H
#define COMMON_H


namespace common {

    template<typename T>
    T clamp(const T& v, const T& low, const T& high) {
        return v < low ? low : (v > high ? high : v);
    }

}

#endif // COMMON_H