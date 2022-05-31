#include "big_num.h"

#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>

static std::mt19937 &mt() {
    // initialize once per thread
    thread_local static std::mt19937 mt(static_cast<uint32_t>(time(nullptr)));
    return mt;
}

BigNum::BigNum(std::size_t maxLen, bool randomKey) {
    if (maxLen < 0) {
        std::cout << "TODO1: error" << '\n';
    }
    factors_.reserve(maxLen);

    if (randomKey) {
        thread_local static std::mt19937 mersenne = mt();
        factors_.resize(maxLen);
        for (auto &factor: factors_) {
            factor = mersenne();
        }

        deleteZeros();
    } else {
        factors_.resize(maxLen);
        std::fill(factors_.begin(), factors_.end(), 0);
    }
}

BigNum::BigNum(std::string str) {
    if (str.length() > 0) {
        size_t lenStr = str.length();

        factors_.clear();
        factors_.reserve(1);
        factors_.resize(1);
        factors_[0] = 0;

        size_t i = 0;
        base_t base = 10;

        //check argument
        for (size_t j = 0; j < lenStr; j++) {
            if (str[j] < '0' || str[j] > '9') {
                throw std::invalid_argument("Invalid argument in constructor from string!");
            }
        }

        while (i < lenStr) {
            *this = *this * base + base_t(str[i] - '0');
            i++;
        }
    }
}

BigNum::BigNum(const BigNum &c1) {
    //factors_.reserve(c1.factors_.capacity());
    //factors_.resize(c1.factors_.size());
    factors_ = c1.factors_;
}

void BigNum::deleteZeros() {
    for (ssize_t i = ssize_t(this->factors_.size()) - 1;
         i >= 0; i--) {  // until 1, not 0 so that 0 is represented as {0} and not {}
        if (this->factors_.size() != 1 && this->factors_[i] == 0) {
            this->factors_.erase(factors_.end() - 1);  // pops all zeroes to the right of the number.
            //std::cout << "cap " << factors_.size() << '\n';
            //std::cout << "size " << factors_.capacity() << '\n';
        } else {
            break;
        }
    }
}

bool checkStr(const std::string &input) {
    return std::all_of(input.begin(), input.end(), [](char i) {
        if ('0' > i || (i > '9' && 'A' > i)) {
            return false;
        }
        if ('f' < i || (i < 'a' && 'F' < i)) {
            return false;
        }
        return true;
    });

    /*for (auto i : input) {
        if ('0' > i || (i > '9' && 'A' > i)) {
            return false;
        }
        else if ('f' < i || (i < 'a' && 'F' < i)) {
            return false;
        }
    }
    return true;*/
}

BigNum &BigNum::operator=(const BigNum &c1) {
    if (this != &c1) {
        factors_.clear();
        this->factors_ = c1.factors_;
//        factors_.reserve(c1.factors_.capacity());
//        factors_.resize(c1.factors_.size());
//
//        for (int i = 0; i < c1.factors_.size(); ++i) {
//            factors_[i] = c1.factors_[i];
//        }
    }

    return *this;
}

int BigNum::bnCompare(const BigNum &bn1, const BigNum &bn2) {
    if (bn1.factors_.size() > bn2.factors_.size()) {
        return 1;
    } else if (bn1.factors_.size() < bn2.factors_.size()) {
        return -1;
    } else {
        for (ssize_t i = ssize_t(bn1.factors_.size()) - 1; i >= 0; i--) {
            if (bn1.factors_[i] > bn2.factors_[i]) {
                return 1;
            } else if (bn1.factors_[i] < bn2.factors_[i]) {
                return -1;
            }
        }
        return 0;
    }
}

bool BigNum::operator>(const BigNum &bn) const {
    return bnCompare(*this, bn) == 1;
}

bool BigNum::operator<(const BigNum &bn) const {
    return bnCompare(*this, bn) == -1;
}

bool BigNum::operator>=(const BigNum &bn) const {
    return bnCompare(*this, bn) != -1;
}

bool BigNum::operator<=(const BigNum &bn) const {
    return bnCompare(*this, bn) != 1;
}

bool BigNum::operator==(const BigNum &bn) const {
    return bnCompare(*this, bn) == 0;
}

bool BigNum::operator!=(const BigNum &bn) const {
    return bnCompare(*this, bn) != 0;
}

BigNum BigNum::operator*(base_t n) const {
    if (n == 0) {
        BigNum res;
        return res;
    }

    BigNum res(this->factors_.size() + 1);
    base_t carry = 0;
    d_base temp;
    size_t i;

    for (i = 0; i < factors_.size(); ++i) {
        temp = carry + factors_[i] * n;
        res.factors_[i] = (base_t) temp;    // отбрасываем старшие байты, то есть делим с остатком на 65536
        carry = base_t(temp >> BASE_SIZE);  // division on 16bit
    }

    res.factors_[i] = carry;
    res.deleteZeros();

    return res;
}

BigNum BigNum::operator*(const BigNum &bn) const {
    BigNum null(0, true);
    if (*this == null) {
        return null;
    }
    int j = 0;
    d_base tmp;
    BigNum res(factors_.size() + bn.factors_.size());
    while (j < bn.factors_.size()) {
        if (bn.factors_[j] == 0) {
            j++;
            continue;
        }
        int i = 0;
        base_t k = 0;

        while (i < factors_.size()) {
            tmp = d_base(factors_[i]) * d_base(bn.factors_[j]) + d_base(res.factors_[i + j]) +
                  d_base(k);
            res.factors_[i + j] = tmp;
            k = tmp >> BASE_SIZE;
            i++;
        }
        res.factors_[j + factors_.size()] = k;
        j++;
    }

    //res.factors_.resize(res.factors_.capacity());
    res.deleteZeros();

    return res;
}

BigNum &BigNum::operator*=(const BigNum &bn) {
    *this = *this * bn;
    return *this;
}

BigNum BigNum::operator+(base_t b) const {
    base_t carry = 0;
    d_base tmp;

    BigNum res(factors_.size() + 1);

    size_t i = 0;
    tmp = factors_[0] + b;
    res.factors_[0] = base_t(tmp);
    carry = base_t(tmp >> BASE_SIZE);
    ++i;

    for (; i < factors_.size(); i++) {
        tmp = carry + factors_[i];
        res.factors_[i] = base_t(tmp);  //делим tmp на 65526 с остатком
        carry = base_t(carry >> BASE_SIZE); // division on 16
    }

    res.factors_[i] = carry;
    res.deleteZeros();

    return res;
}

BigNum BigNum::operator+(const BigNum &bn) const {
    ssize_t min_len = factors_.size() < bn.factors_.size() ? ssize_t(factors_.size()) : ssize_t(bn.factors_.size());
    ssize_t j;
    BigNum result(factors_.size() + bn.factors_.size() - min_len + 1);
    d_base tmp;
    base_t k = 0;
    for (j = 0; j < min_len; j++) {
        tmp = factors_[j] + bn.factors_[j] + k;
        result.factors_[j] = (base_t) tmp;
        k = (base_t) (tmp >> BASE_SIZE);
    }
    while (j < factors_.size()) {
        tmp = factors_[j] + k;
        result.factors_[j] = (base_t) tmp;
        k = (base_t) (tmp >> BASE_SIZE);
        j++;
    }
    while (j < bn.factors_.size()) {
        tmp = bn.factors_[j] + k;
        result.factors_[j] = (base_t) tmp;
        k = (base_t) tmp >> BASE_SIZE;
        j++;
    }

    result.factors_[j] = k;
    result.factors_.resize(result.factors_.capacity() - !k);
    //result.deleteZeros();
    return result;
}

BigNum BigNum::operator+=(const BigNum &bn) {
    *this = *this + bn;
    return *this;
}

BigNum BigNum::operator-(const BigNum &bn) const {
    if (*this < bn)
        throw std::invalid_argument("Invalid operand in operator -");
//    if(*this == nullptr){
//        std::cout << "err in -" << '\n';
//    }
    BigNum result(factors_.size());
    d_base tmp, k = 0;
    int j;
    d_base b = 1 << BASE_SIZE;
    for (j = 0; j < bn.factors_.size(); j++) {
        tmp = b | factors_[j];
        tmp -= bn.factors_[j] + k;
        result.factors_[j] = (base_t) tmp;
        k = !(tmp >> BASE_SIZE);
    }
    for (; j < factors_.size(); j++) {
        tmp = (factors_[j] - k) | b;
        result.factors_[j] = (base_t) tmp;
        k = !(tmp >> BASE_SIZE);
    }
    //result.factors_.size() = result.factors_.capacity();
    result.deleteZeros();
    return result;
}

BigNum &BigNum::operator-=(const BigNum &bn) {
    *this = *this - bn;
    return *this;
}

BigNum BigNum::operator/(base_t num) const {
    if (num == 0) {
        throw std::invalid_argument("Division by zero in operator operator /");
    }
    BigNum result(factors_.size());
    //base_t r = 0;
    d_base tmp, r = 0;
    for (size_t j = 0; j < factors_.size(); j++) {
        tmp = (d_base(r) << BASE_SIZE) + d_base(factors_[factors_.size() - 1 - j]);
        result.factors_[factors_.size() - 1 - j] = tmp / d_base(num);
        r = tmp % d_base(num);
    }
    //result.len = result.maxLen;
    result.deleteZeros();
    return result;
}

BigNum BigNum::operator/(const BigNum &bn) const {
    if (bn.factors_[0] == 0 && bn.factors_.size() == 1) {
        throw std::invalid_argument("Division by zero in operator BigNum::operator/");
    }
    if (*this == BigNum(0, true)) {
        return BigNum(0, true); //return empty number
    }
    if (*this < bn) {
        BigNum res;
        return res; //return 0
    }
    if (*this == bn) {
        BigNum res;
        res.factors_[0] = 1;
        return res; // return 1
    }
    if (bn.factors_.size() == 1) {
        base_t num = bn.factors_[0];
        BigNum res;
        res = *this / num;
        return res;
    }

    BigNum thisCopy = *this;    //делимое
    BigNum bnCopy = bn; // делитель

    auto nBnSize = ssize_t(bnCopy.factors_.size());
    ssize_t mThisMinusBn = ssize_t(thisCopy.factors_.size()) - nBnSize;

    //normalize
    base_t d = base / (bnCopy.factors_[nBnSize - 1] + 1);

    thisCopy = thisCopy * d;
    bnCopy = bnCopy * d;

    //дозапись в начало делимого 0
    if (thisCopy.factors_.size() - nBnSize == mThisMinusBn) {
        BigNum newNum(thisCopy.factors_.size() + 1);
        for (int i = 0; i < thisCopy.factors_.size(); i++) {
            newNum.factors_[i] = thisCopy.factors_[i];
        }
        newNum.factors_[thisCopy.factors_.size()] = 0;
        thisCopy.factors_.clear();
        thisCopy = newNum;
    }

    //начальная установка
    ssize_t j = mThisMinusBn;
    bool flag = false;
    BigNum q(size_t(mThisMinusBn) + 1);
    d_base qTemp, rTemp;

    while (j >= 0) {
        qTemp = ((d_base2(thisCopy.factors_[j + nBnSize]) << BASE_SIZE) + d_base2(thisCopy.factors_[j + nBnSize - 1])) /
                d_base2(bnCopy.factors_[nBnSize - 1]);
        rTemp = ((d_base2(thisCopy.factors_[j + nBnSize]) << BASE_SIZE) + d_base2(thisCopy.factors_[j + nBnSize - 1])) %
                d_base2(bnCopy.factors_[nBnSize - 1]);

        if (qTemp == (d_base(1) << BASE_SIZE) ||
            qTemp * bnCopy.factors_[nBnSize - 2] >
            (d_base(1) << BASE_SIZE) * rTemp + thisCopy.factors_[j + nBnSize - 2]) {
            qTemp--;
            rTemp = rTemp + bnCopy.factors_[nBnSize - 1];
        }

        if (rTemp < (d_base(1) << BASE_SIZE)) {
            if (qTemp == (d_base(1) << BASE_SIZE) ||
                qTemp * bnCopy.factors_[nBnSize - 2] >
                (d_base(1) << BASE_SIZE) * rTemp + thisCopy.factors_[j + nBnSize - 2]) {
                qTemp--;
            }
        }

        BigNum uTemp(nBnSize + 1);
        for (ssize_t i = j; i <= j + nBnSize; i++) {
            uTemp.factors_[i - j] = thisCopy.factors_[i];
        }

        BigNum vTemp;
        vTemp = bnCopy * base_t(qTemp);

        if (uTemp < vTemp) {
            BigNum newNum(uTemp.factors_.size() + 1);
            for (ssize_t i = 0; i < uTemp.factors_.size(); i++) {
                newNum.factors_[i] = uTemp.factors_[i];
            }
            newNum.factors_[uTemp.factors_.size()] = 1;
            uTemp.factors_.clear();
            uTemp = newNum;
            flag = true;
        } else {
            flag = false;
        }

        uTemp = uTemp - vTemp;
        auto pred_len = ssize_t(uTemp.factors_.size());
        q.factors_[j] = qTemp;

        if (flag) {
            q.factors_[j]--;
            uTemp += bnCopy;
        }

        for (ssize_t i = j; i <= j + nBnSize; i++) {
            if (i - j > pred_len - 1) {
                thisCopy.factors_[i] = 0;
                continue;
            }
            thisCopy.factors_[i] = uTemp.factors_[i - j];
        }
        thisCopy.deleteZeros();
        j--;
    }

    BigNum res;
    res.factors_.clear();
    res = q;

    return res;
}

BigNum &BigNum::operator/=(const BigNum &bn) {
    *this = *this / bn;
    return *this;
}

BigNum BigNum::operator%(base_t num) const {
    if (num == 0) {
        throw std::invalid_argument("Division by zero in % on base");
    }
    BigNum res;
    d_base tmp, rem = 0;

    ssize_t j = 0;
    while (j < factors_.size()) {
        tmp = ((d_base(rem)) << BASE_SIZE) + d_base(factors_[factors_.size() - 1 - j]);
        rem = tmp % d_base(num);
        j++;
    }

    res.factors_[0] = rem;

    res.deleteZeros();
    return res;
}

BigNum BigNum::operator%(const BigNum &other) const {
    //частные случаи
    if (other.factors_[0] == 0 && other.factors_.size() == 1) {
        throw std::invalid_argument("Division by zero in % on base");
    }
    if (*this < other) {
        BigNum res = *this;
        return res;
    }
    if (*this == other) {
        BigNum res;
        return res;
    }
    if (other.factors_.size() == 1) {
        base_t num = other.factors_[0];
        BigNum res;
        res = *this % num;
        return res;
    }
    //делимое
    BigNum u = *this;

    //делитель
    BigNum v = other;

    //длинна делителя
    auto n = ssize_t(v.factors_.size());

    //длина делимого
    ssize_t m = ssize_t(u.factors_.size()) - n;

    //шаг D1 нормализация
    d_base d;
    d = (d_base(1) << BASE_SIZE) / (d_base(v.factors_[n - 1]) + d_base(1));

    u = u * base_t(d);
    v = v * base_t(d);

    //дозапись в начало делимого 0
    if (u.factors_.size() - n == m) {
        BigNum newNum(u.factors_.size() + 1);
        for (ssize_t i = 0; i < u.factors_.size(); i++) {
            newNum.factors_[i] = u.factors_[i];
        }
        newNum.factors_[u.factors_.size()] = 0;
        u.factors_.clear();
        u = newNum;
        //u.capacity_++;
        //u.size_++;
    }

    //начальная установка
    ssize_t j = m;
    bool flag = false;
    BigNum q(m + 1);
    d_base _q, _r;

    //цикл по j, дает один разряд частного по j
    while (j >= 0) {
        //вычисляем приблеженный остаток и частное
        _q = ((d_base(u.factors_[j + n]) << BASE_SIZE) + d_base(u.factors_[j + n - 1])) /
             d_base(v.factors_[n - 1]);
        _r = ((d_base(u.factors_[j + n]) << BASE_SIZE) + d_base(u.factors_[j + n - 1])) %
             d_base(v.factors_[n - 1]);

        if (_q == (d_base(1) << BASE_SIZE) ||
            _q * v.factors_[n - 2] >
            (d_base(1) << BASE_SIZE) * _r + u.factors_[j + n - 2]) {
            _q--;
            _r = _r + v.factors_[n - 1];
        }

        if (_r < (d_base(1) << BASE_SIZE)) {
            if (_q == (d_base(1) << BASE_SIZE) ||
                _q * v.factors_[n - 2] >
                (d_base(1) << BASE_SIZE) * _r + u.factors_[j + n - 2]) {
                _q--;
            }
        }

        //из соответствующей части делимого вычитаем делитель умноженный на _q
        BigNum u_temp(n + 1);
        for (ssize_t i = j; i <= j + n; i++) {
            u_temp.factors_[i - j] = u.factors_[i];
        }

        //u_temp.factors_.size() = n + 1;
        BigNum v_temp;
        v_temp = v * base_t(_q);

        //проверяем левая часть меньше правой если да
        //то сделаем займ (добавим слева 1)
        if (u_temp < v_temp) {
            BigNum newNum(u_temp.factors_.size() + 1);
            for (int i = 0; i < u_temp.factors_.size(); i++) {
                newNum.factors_[i] = u_temp.factors_[i];
            }
            newNum.factors_[u_temp.factors_.size()] = 1;
            u_temp.factors_.clear();
            u_temp = newNum;
            //u_temp.capacity_++;
            //u_temp.size_++;
            flag = true;
        } else {
            flag = false;
        }

        u_temp = u_temp - v_temp;
        ssize_t pred_len = u_temp.factors_.size();
        q.factors_[j] = _q;

        if (flag) {
            //компенсация сложения
            q.factors_[j]--;
            u_temp += v;
        }

        for (ssize_t i = j; i <= j + n; i++) {
            if (i - j > pred_len - 1) {
                u.factors_[i] = 0;
                continue;
            }
            u.factors_[i] = u_temp.factors_[i - j];
        }
        u.deleteZeros();
        j--;
    }

    u = u / base_t(d);

    return u;
}

BigNum BigNum::fastSqr() const {
    BigNum res(2 * factors_.size() + 1); // because after ZERO-fill size will be automatically trimmed to 1

    for (size_t i = 0; i < factors_.size(); i++) {
        d_base2 cuv = res.factors_[2 * i] + d_base2(factors_[i]) * factors_[i];
        res.factors_[2 * i] = cuv;

        for (size_t j = i + 1; j < factors_.size(); j++) {
            cuv = res.factors_[i + j] + d_base2(2) * factors_[i] * factors_[j] + (cuv >> BASE_SIZE);
            res.factors_[i + j] = cuv;
        }

//      cuv >>= base_size;
//      cuv += d_base2(res.factors_[i + factors_.size()]);
//      tmp += d_base2(res.factors_[i + len + 1]) << BASE_SIZE;
//      res.factors_[i + factors_.size()] = cuv;
//      res.factors_[i + factors_.size() + 1] = cuv >> BASE_SIZE;

        auto y = (d_base2 *) &res.factors_[i + factors_.size()];
        *y += cuv >> BASE_SIZE;
    }

    res.deleteZeros();
    BigNum null(0, true);
    if (res == BigNum(1)) {
        return null;
    }
    return res;
}

BigNum BigNum::power(const BigNum &pow) {
//    if (!(Pow.factors_[0] & 1)) {
//        return BigNum(0);
//    }

    int bits = (pow.factors_.size() - 1) * BASE_SIZE;
    int idx = BASE_SIZE - 1;
    while (true) {
        if (pow.factors_[pow.factors_.size() - 1] & (1 << idx)) {
            bits += idx + 1;
            break;
        }
        idx--;
    }

    BigNum z = *this;

    for (ssize_t i = bits - 2; i >= 0; --i) {
        z = z.fastSqr();
        if (pow.factors_[i / BASE_SIZE] & (1 << (i % BASE_SIZE))) {
            z = z * (*this);
        }
    }

    return z;
}

BigNum BigNum::RightShift(size_t shiftSize) const {
    if (shiftSize == 0) {
        return *this;
    }

    size_t newNumberSize = (shiftSize >= factors_.size()) ? 0 : factors_.size() - shiftSize;
    if (newNumberSize == 0) {
        return BigNum(1);
    }

    BigNum res(newNumberSize);
    for (size_t i = shiftSize; i < factors_.size(); ++i) {
        res.factors_[i - shiftSize] = factors_[i];
    }

    return res;
}

BigNum BigNum::LeftShift(size_t &shiftSize) const {
    if (shiftSize == 0) {
        return *this;
    }

    size_t newNumberSize = shiftSize + factors_.size();
    BigNum res(newNumberSize);

    for (size_t i = shiftSize; i < newNumberSize; ++i) {
        res.factors_[i] = factors_[i - shiftSize];
    }

    return res;
}

BigNum BigNum::Mod(const BigNum &m, const BigNum &z) const {
    if (this->factors_.size() > 2 * m.factors_.size()) {
        throw std::invalid_argument("Incorrect size");
    }

    //calculate q'
    BigNum exp;
    exp = this->RightShift(m.factors_.size() - 1);
    exp *= z;
    exp = exp.RightShift(m.factors_.size() + 1);

    BigNum rem1 = *this;
    if (rem1.factors_.size() > m.factors_.size()) {
        rem1.factors_.resize(m.factors_.size());
    }

    BigNum rem2 = exp * m;
    if (rem2.factors_.size() > m.factors_.size()) {
        rem2.factors_.resize(m.factors_.size());
    }

    BigNum res;
    if (rem1 >= rem2) {
        res = rem1 - rem2;
    } else {
        res.factors_.resize(m.factors_.size() + 1);
        res.factors_[m.factors_.size()] = 1;
        res = res + rem1 - rem2;
    }

    while (res >= m) {
        res -= m;
    }

    return res;
}

BigNum FindZ(const BigNum &m) {
    BigNum temp(2 * m.factors_.size() + 1); //b^2k, b = 65536
    temp.factors_[2 * m.factors_.size()] = 1;   //1 0000 ...
    BigNum z;
    z = temp / m;
    z.deleteZeros();

    return z;
}

BigNum BigNum::Pow(const BigNum &pow, const BigNum &mod, const BigNum &barretZ) {
    int bits = (pow.factors_.size() - 1) * BASE_SIZE;
    int idx = BASE_SIZE - 1;
    while (true) {
        if (pow.factors_[pow.factors_.size() - 1] & (1 << idx)) {
            bits += idx + 1;
            break;
        }
        idx--;
    }

    BigNum z = *this;
    BigNum cmp("1");

    for (ssize_t i = bits - 2; i >= 0; --i) {
        z = z.fastSqr();
        z = z.Mod(mod, barretZ);
        if (pow.factors_[i / BASE_SIZE] & (1 << (i % BASE_SIZE))) {
            z = z * (*this);
            z = z.Mod(mod, barretZ);
        }
    }

    return z;
}

void BigNum::TestFerma(const BigNum &num, size_t t) {
    BigNum barretZ = FindZ(num);
    bool flag = true;
    while (t-- > 0) {
        BigNum a(num.factors_.size(), true);
        BigNum cmp("2");
        while (a > (num - cmp) || a < cmp) {
            a = BigNum(num.factors_.size(), true);
        }

        BigNum rem;
        cmp = BigNum("1");
        rem = a.Pow(num - cmp, num, barretZ);
        //rem = rem % num;

        if (rem != cmp) {
            std::cout << "This number is composite!" << '\n';
            flag = false;
            break;
        }
    }
    if (flag) {
        std::cout << "This number is prime" << '\n';
    }
}

int BigNum::Jacobi(const BigNum &a, const BigNum &n) {
    if (a == BigNum(1)) {
        return 0;
    }
    if (a == BigNum("1")) {
        return 1;
    }

    int k = 0;
    int s = 0;
    BigNum aCopy = a;
    while (aCopy % 2 == BigNum()) {
        k = k + 1;
        aCopy = aCopy / 2;
    }
    if (k % 2 == 0 && k > 0) {
        s = 1;
    } else if (n % 8 == BigNum("1") || n % 8 == BigNum("7")) {
        s = 1;
    } else {
        s = -1;
    }

    if (n % 4 == BigNum("3") && aCopy % 4 == BigNum("3")) {
        s = -s;
    }

    if (aCopy == BigNum("1")) {
        return s;
    } else {
        return (s * Jacobi(n % aCopy, aCopy));
    }

}

void BigNum::testStrassen(const BigNum &num, size_t t) {
    BigNum barretZ = FindZ(num);
    bool flag = true;
    while (t-- > 0) {
        BigNum a(num.factors_.size(), true);
        BigNum cmp("2");
        while (a > (num - cmp) || a < cmp) {
            a = BigNum(num.factors_.size(), true);
        }

        BigNum rem;
        cmp = BigNum("1");
        rem = a.Pow((num - cmp) / 2, num, barretZ);

        if (rem != cmp && rem != num - cmp) {
            std::cout << "This number is composite!" << '\n';
            flag = false;
            break;
        }

    }
    if (flag) {
        std::cout << "This number is prime" << '\n';
    }
}

std::istream &operator>>(std::istream &in, BigNum &c1) {
    std::string inputStr;
    in >> inputStr;
    c1.factors_.reserve((inputStr.size() - 1) / (BASE_SIZE / 4) + 1);
    c1.factors_.resize((inputStr.size() - 1) / (BASE_SIZE / 4) + 1);
    std::fill(c1.factors_.begin(), c1.factors_.end(), 0);
    size_t i = 0;
    int n = 0;
    if (!checkStr(inputStr)) {
        throw std::range_error("Bad args");
    }
    for (ssize_t idx_str = ssize_t(inputStr.size()) - 1, k = 0; idx_str >= 0; --idx_str, ++k) {
        if (k >= BASE_SIZE / 4) {
            k = 0;
            ++i;
        }
        if ('0' <= inputStr[idx_str] && inputStr[idx_str] <= '9') {
            n = inputStr[idx_str] - '0';
        } else if ('a' <= inputStr[idx_str] && inputStr[idx_str] <= 'f') {
            n = inputStr[idx_str] - 'a' + 10;
        } else if ('A' <= inputStr[idx_str] && inputStr[idx_str] <= 'F') {
            n = inputStr[idx_str] - 'A' + 10;
        }
        c1.factors_[i] |= (n << (k * 4));
    }

    c1.deleteZeros();

    return in;
}

std::ostream &operator<<(std::ostream &out, const BigNum &c1) {
    for (ssize_t i = ssize_t(c1.factors_.size()) - 1; i >= 0; --i) {
        std::cout.width(BASE_SIZE / 4);
        std::cout.fill('0');
        std::cout << std::hex << std::uppercase << c1.factors_[i];
    }
    return out;
}