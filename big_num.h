#ifndef BIG_NUM_H
#define BIG_NUM_H
#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

typedef uint16_t base_t;
typedef uint64_t d_base;
typedef int64_t d_base2;
#define BASE_SIZE (sizeof(base_t) * 8)

const uint64_t base = (2LL << (BASE_SIZE - 1));

//0  1
//b6 21
//21b6

class BigNum {
    std::vector<base_t> factors_;
    //bool positive_;
public:

    ///Constructors

    explicit BigNum(size_t maxLen = 1, bool randomKey = false);

    explicit BigNum(std::string);

    BigNum(const BigNum &);

    ///Allocation

    BigNum &operator=(const BigNum &);

    ///Compare
    static int bnCompare(const BigNum &, const BigNum &);

    bool operator>(const BigNum &) const;

    bool operator<(const BigNum &) const;

    bool operator>=(const BigNum &) const;

    bool operator<=(const BigNum &) const;

    bool operator==(const BigNum &) const;

    bool operator!=(const BigNum &) const;

    ///Helpers

    void deleteZeros();

    ///Multiplication

    BigNum operator*(base_t n) const;

    BigNum operator*(const BigNum &) const;

    BigNum &operator*=(const BigNum &);

    ///Adding

    BigNum operator+(base_t b) const;

    BigNum operator+(const BigNum &) const;

    BigNum operator+=(const BigNum &);
    ///Subtraction

    BigNum operator-(const BigNum &) const;

    BigNum &operator-=(const BigNum &);

    ///Division

    BigNum operator/(base_t num) const;

    BigNum operator/(const BigNum &) const;

    BigNum &operator/=(const BigNum &bn);

    BigNum operator%(base_t num) const;

    BigNum operator%(const BigNum &other) const;

    //Fast sqr
    BigNum FastSqr() const;

    BigNum Power(const BigNum &pow);

    BigNum Pow(const BigNum &pow, const BigNum &mod, const BigNum &barretZ);

    BigNum RightShift(size_t shiftSize) const;

    BigNum LeftShift(size_t &shiftSize) const;

    BigNum Mod(const BigNum &m, const BigNum &z) const;

    friend BigNum FindZ(const BigNum &m);

    static bool TestFerma(const BigNum &num, size_t t);

    static int Jacobi(const BigNum &a, const BigNum &n);

    static bool TestStrassen(const BigNum &num, size_t t);

    BigNum Phi();

    float ConvertToFloat();

    static BigNum GeneratePrimeNumber(size_t );

    static BigNum StrongGordonPrime();
    ///input_Output

    friend std::ostream &operator<<(std::ostream &, const BigNum &);

    friend std::istream &operator>>(std::istream &, BigNum &);
};

#endif