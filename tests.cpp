#include "big_num.h"
#include <iostream>
#include <ctime>
#include <random>
#include <chrono>

void testsConstructors() {
    BigNum zero;
    std::cout << "Zero: " << zero << '\n';

    BigNum random(3, true);
    std::cout << "Random: " << random << '\n';

    BigNum in;
    std::cout << "Enter number in hex: ";
    std::cin >> in;
    std::cout << "Got: " << in << '\n';

    const BigNum &copy = random;
    std::cout << "Copy of random: " << copy << '\n';

    zero = random;
    std::cout << "Assigned from random: " << zero << '\n';

    std::string str = "1274893458972133697412384734646510022367468487181273664558503732647237618612";
    BigNum from_str(str);
    std::cout << "From string \"" << str << "\": " << from_str << '\n';
}

void testsCompare() {
    ssize_t N = 100;
    while (N-- > 0) {
        BigNum f;
        std::cin >> f;

        BigNum s;
        std::cin >> s;

        if (f > s) {
            std::cout << f << ">" << s << '\n';
        }

        if (f >= s) {
            std::cout << f << ">=" << s << '\n';
        }

        if (f == s) {
            std::cout << f << "==" << s << '\n';
        }

        if (f != s) {
            std::cout << f << "!=" << s << '\n';
        }

        if (f <= s) {
            std::cout << f << "<=" << s << '\n';
        }

        if (f < s) {
            std::cout << f << "<" << s << '\n';
        }
    }
}

void testsMul() {
    int N = 1000;

    std::random_device dev;
    std::mt19937 mt(dev());
    while (N > 0) {
        BigNum f(mt() % 20, true);
        BigNum s(mt() % 20, true);

        BigNum f_copy = f;

        f *= s;
        try {
            f /= s;

        } catch (const std::invalid_argument &e) {
            continue;
        }
        if (f_copy != f) {
            std::cout << "Failed test with: " << '\n';
            std::cout << "f_copy: " << f_copy << '\n';
            std::cout << "f: " << f << '\n';
            std::cout << "s: " << s << '\n';
            std::cout << "res: " << f << '\n' << '\n';
        }
        --N;
    }
}

void testsSum() {
    ssize_t N = 100000;

    std::random_device dev;
    std::mt19937 mt(dev());
    while (N-- > 0) {
        try {
            BigNum f(mt() % 1000, true);
            BigNum s(mt() % 1000, true);

            if (f < s) {
                BigNum t = f;
                f = s;
                s = t;
            }

            BigNum copy = f;

            f -= s;
            f += s;

            if (copy != f) {
//                std::cout << "f: " << copy << '\n';
//                std::cout << "s: "<< s << '\n';
//                std::cout << "res: "<< f << '\n';
                throw std::invalid_argument("Saved value != result!");
            }

            f = (f - s) + s;
            if (copy != f) {
                throw std::invalid_argument("Saved value != result v2!");
            }
        } catch (const std::invalid_argument &e) {
            std::cout << e.what() << '\n';
        }
    }
}

void testsAllOperations() {
    int T = 1000, good_count = 0;
    BigNum A, B, C, D;
    std::random_device dev;
    std::mt19937 mt(dev());
    while (T--) {
        BigNum AA(mt() % 2000, true),
                BB(mt() % 2000, true);
        while (BB == BigNum(0, true)) {
            BB = BigNum(mt() % 2000, true);
        }
        A = AA;
        B = BB;
        C = A / B;
        D = A % B;
        if (AA >= BB) {
            if (A == C * B + D && A - D == C * B && D < B) {
                good_count++;
            } else {
                std::cout << "AA  " << AA << '\n';
                std::cout << "BB " << BB << '\n';
            }
        } else {
            if (A == C * B + D && D < B) {
                good_count++;
            } else {
                std::cout << "AA in second " << AA << '\n';
                std::cout << "BB in second " << BB << '\n';
            }
        }
        //cout << "T " << T << endl;
        //cout << "n " << n << endl;
        //cout << "m " << m << endl;
    }
    if (good_count == 1000) {
        std::cout << "Good" << '\n';
    } else {
        std::cout << "BAD" << '\n';
    }
}

void testsShift() {
    ssize_t N = 1000;

    std::random_device dev;
    std::mt19937 mt(dev());

    while (N-- > 0) {
        BigNum bn(mt() % 6000, true);
        BigNum bnCopy;
        bnCopy = bn;
        size_t shiftSize = mt() % 6000;

        bn.RightShift(shiftSize);
        bn.LeftShift(shiftSize);

        if (bn != bnCopy) {
            std::cout << "Failed test!" << '\n';
            std::cout << bnCopy << '\n';
        }
    }
}

void testsFastSqr() {
    thread_local static std::mt19937 mt(static_cast<uint32_t>(time(nullptr)));

    {
        std::cout << "Test random numbers: " << '\n';
        ssize_t N = 300;
        while (N-- > 0) {
            BigNum f(1000, true);
            BigNum fFastSqur = f.FastSqr();
            BigNum fMulf = f * f;
            if (fFastSqur != fMulf) {
                std::cout << "Failed test with: " << '\n';
                std::cout << "fFastSqr: " << fFastSqur << '\n';
                std::cout << "   fMulf: " << fMulf << '\n';
                std::cout << "f: " << f << '\n' << '\n';
                break;
            }
        }
    }

    {
        std::cout << "Test for random number with 2 length: " << '\n';
        BigNum f(2, true);
        BigNum fFastSqr = f.FastSqr();
        BigNum fMulf = f * f;
        if (fMulf != fFastSqr) {
            std::cout << "Failed test with: " << '\n';
            std::cout << "fFastSqr: " << fFastSqr << '\n';
            std::cout << "   fMulf: " << fMulf << '\n';
            std::cout << "f: " << f << '\n' << '\n';
        }
    }

    {
        std::cout << "Test for random number with 1 length: " << '\n';
        BigNum f(1, true);
        BigNum fFastSqr = f.FastSqr();
        BigNum fMulf = f * f;
        if (fMulf != fFastSqr) {
            std::cout << "Failed test with: " << '\n';
            std::cout << "fFastSqr: " << fFastSqr << '\n';
            std::cout << "   fMulf: " << fMulf << '\n';
            std::cout << "f: " << f << '\n' << '\n';
        }
    }

    {
        std::cout << "Test for random number with 0 length: " << '\n';
        BigNum f(0);
        BigNum fFastSqr = f.FastSqr();
        BigNum fMulf = f * f;
        if (fMulf != fFastSqr) {
            std::cout << "Failed test with: " << '\n';
            std::cout << "fFastSqr: " << fFastSqr << '\n';
            std::cout << "   fMulf: " << fMulf << '\n';
            std::cout << "f: " << f << '\n' << '\n';
        }
    }

    {
        using std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;
        using std::chrono::duration;
        using std::chrono::milliseconds;

        ssize_t N = 30;
        ssize_t M = 5000;
        auto t1 = high_resolution_clock::now();
        for (size_t i = 0; i < N; ++i) {
            BigNum f(M, true);
            BigNum fSqr = f.FastSqr();
        }
        auto t2 = high_resolution_clock::now();
        auto ms_int = duration_cast<milliseconds>(t2 - t1);
        std::cout << "FastSqr: time taken for " << N << " numbers to square: " << ms_int.count() << '\n';

        t1 = high_resolution_clock::now();
        for (size_t i = 0; i < N; ++i) {
            BigNum f(M, true);
            BigNum fSqr = f * f;
        }
        t2 = high_resolution_clock::now();
        ms_int = duration_cast<milliseconds>(t2 - t1);
        std::cout << "SlowSqr: time taken for " << N << " numbers to square: " << ms_int.count() << '\n';


    }

    {
        std::cout << "Interactive: " << '\n';
        ssize_t N = 5;
        while (N-- > 0) {
            BigNum f;
            std::cin >> f;

            BigNum fSqur = f.FastSqr();
            std::cout << fSqur << '\n';
        }
    }
}

void testsPow() {
    thread_local static std::mt19937 mt(static_cast<uint32_t>(time(nullptr)));

//    {
//        std::cout << "Test random numbers: " << '\n';
//        ssize_t N = 5;
//        while (N-- > 0) {
//            BigNum f(1, true);
//            BigNum g(1, true);
//
//            std::cout << "f: " << f << '\n';
//            std::cout << "g: " << g << '\n';
//
//            BigNum fPower = f.Power(g);
//            BigNum fStupidPower = f.stupidPow(g);
//            if (fPower != fStupidPower) {
//                std::cout << "Failed test with: " << '\n';
//                std::cout << "      fPow: " << fPower << '\n';
//                std::cout << "fStupidPow: " << fStupidPower << '\n';
//                std::cout << "  f: " << f << '\n' << '\n';
//                std::cout << "Pow: " << g << '\n' << '\n';
//                break;
//            }
//        }
//        std::cout << "Test with random numbers is passed" << '\n';
//    }
//
//    {
//        std::cout << "Test for random number with 2 length: " << '\n';
//        BigNum f(2, true);
//        BigNum fFastSqr = f.FastSqr();
//        BigNum fMulf = f * f;
//        if (fMulf != fFastSqr) {
//            std::cout << "Failed test with: " << '\n';
//            std::cout << "fFastSqr: " << fFastSqr << '\n';
//            std::cout << "   fMulf: " << fMulf << '\n';
//            std::cout << "f: " << f << '\n' << '\n';
//        }
//    }
//
//    {
//        std::cout << "Test for random number with 1 length: " << '\n';
//        BigNum f(1, true);
//        BigNum fFastSqr = f.FastSqr();
//        BigNum fMulf = f * f;
//        if (fMulf != fFastSqr) {
//            std::cout << "Failed test with: " << '\n';
//            std::cout << "fFastSqr: " << fFastSqr << '\n';
//            std::cout << "   fMulf: " << fMulf << '\n';
//            std::cout << "f: " << f << '\n' << '\n';
//        }
//    }
//
//    {
//        std::cout << "Test for random number with 0 length: " << '\n';
//        BigNum f(0);
//        BigNum fFastSqr = f.FastSqr();
//        BigNum fMulf = f * f;
//        if (fMulf != fFastSqr) {
//            std::cout << "Failed test with: " << '\n';
//            std::cout << "fFastSqr: " << fFastSqr << '\n';
//            std::cout << "   fMulf: " << fMulf << '\n';
//            std::cout << "f: " << f << '\n' << '\n';
//        }
//    }
//
//    {
//        using std::chrono::high_resolution_clock;
//        using std::chrono::duration_cast;
//        using std::chrono::duration;
//        using std::chrono::milliseconds;
//
//        ssize_t N = 30;
//        ssize_t M = 2;
//        auto t1 = high_resolution_clock::now();
//        for(size_t i = 0; i < N; ++i) {
//            BigNum f(M, true);
//            BigNum g(M, true);
//            BigNum fPow = f.Power(g);
//        }
//        auto t2 = high_resolution_clock::now();
//        auto ms_int = duration_cast<milliseconds>(t2 - t1);
//        std::cout << "FastSqr: time taken for " << N << " numbers to Pow: " << ms_int.count() << '\n';
//
//        t1 = high_resolution_clock::now();
//        for(size_t i = 0; i < N; ++i) {
//            BigNum f(M, true);
//            BigNum g(M, true);
//            BigNum fPow = f.stupidPow(g);
//        }
//        t2 = high_resolution_clock::now();
//        ms_int = duration_cast<milliseconds>(t2 - t1);
//        std::cout << "SlowSqr: time taken for " << N << " numbers to square: " << ms_int.count() << '\n';
//
//
//    }

    {
        std::cout << "Interactive: " << '\n';
        ssize_t N = 5;
        while (N-- > 0) {
            BigNum f, g;
            std::cout << "Enter f: ";
            std::cin >> f;
            std::cout << "Enter Pow: ";
            std::cin >> g;

            std::cout << "Pow: " << f.Power(g) << '\n';
        }
    }
}

void testsBarrett() {
    int N = 10;

    std::random_device dev;
    std::mt19937 mt(dev());

    while (N > 0) {
        BigNum x(mt() % 1000, true);
        BigNum m(mt() % 2000, true);

        BigNum rem = x % m;
        try {
            BigNum mod = x.Mod(m, FindZ(m));

            if (mod != rem) {
                std::cout << "Failed test: " << '\n';
                std::cout << "x: " << x << '\n';
                std::cout << "m: " << m << '\n';
                std::cout << " x%m: " << rem << '\n';
                std::cout << " x Mod m: " << mod << '\n';
            }
        } catch (std::invalid_argument &e) {
        }
        std::cout << N << '\n';
        --N;
    }

    N = 100;

    BigNum m(mt() % 800, true);
    BigNum z = FindZ(m);

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {
        BigNum x(mt() % 1000, true);
        try {
            BigNum mod = x.Mod(m, z);
        } catch (const std::invalid_argument &e) {
            continue;
        }

    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    std::cout << "It took me calculate Mod: " << time_span.count() << '\n';

    t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {
        BigNum x(mt() % 1000, true);
        BigNum mod = x % m;
    }
    t2 = std::chrono::steady_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    std::cout << "It took me calculate %: " << time_span.count() << '\n';

    //interactive test
    N = 5;
    while (N-- > 0) {
        BigNum x;
        std::cin >> m >> x;
        z = FindZ(m);
        if (x.Mod(m, z) == x % m) {
            std::cout << "Res: " << x.Mod(m, z) << '\n';
        }
    }

}

void testsFerma() {

//    ssize_t N = 10;
//    while (N-- > 0) {
//        BigNum num(1, true);
//        BigNum cmp("3");
//
//        while (num % 2 == BigNum() && num <= cmp) {
//            num = BigNum(1, true);
//        }
//        std::cout << num << '\n';
//        BigNum::TestFerma(num, 5);
//    }

    std::cout << "Test #2" << '\n';
    BigNum num("811");
    BigNum::TestFerma(num, 100);
    BigNum err = num.Phi() * BigNum("100")/num;
    float error = err.ConvertToFloat() / 100;
    std::cout << pow(error, 100) << '\n';
}

void TestJacobi() {
    BigNum a, n;
    ssize_t N = 5;
    while (N-- > 0) {
        std::cout << "Enter a: ";
        std::cin >> a;
        std::cout << "Enter n: ";
        std::cin >> n;

        std::cout << BigNum::Jacobi(a, n) << '\n';
    }
}

void TestPhi() {
    BigNum num("849228304920");
    std::cout << num.Phi() << '\n';
}

void TestsStrassen() {
    ssize_t N = 10;
    while (N-- > 0) {
        BigNum num(1, true);
        BigNum cmp("3");

        while (num % 2 == BigNum() && num <= cmp) {
            num = BigNum(1, true);
        }
        std::cout << num << '\n';
        BigNum::TestStrassen(num, 10);
    }

    std::cout << "Test #2" << '\n';
    BigNum num("811");
    BigNum::TestStrassen(num, 100);
    BigNum err = num.Phi() * BigNum("100")/num;
    float error = err.ConvertToFloat() / 100;
    std::cout << pow(error, 100) << '\n';
}

int main() {
    //testsConstructors();
    //testsCompare();
    //testsSum();
    //testsMul();
    //testsAllOperations();
    //testsShift();
    //testsBarrett();
    //testsFastSqr();
    //testsFerma();
    //TestJacobi();
    //TestPhi();
    //TestsStrassen();

    //test fo generatin ptime number
//    BigNum res = BigNum::StrongGordonPrime();
//    std::cout << res << '\n';
//    std::cout << (BigNum::TestStrassen(res, 10) == 1 ? "This is prime" : "This is not prime") << '\n';

//    BigNum n("248113963299781512916613733151690498049");
//    //test for strassen
//    while (true) {
//        std::cout << BigNum::TestStrassen(n, 100) << '\n';
//    }
    //test for ferma
    //std::cout << BigNum::TestFerma(n, 100) << '\n';

    BigNum t("6749120");
    BigNum m("248113963299781512916613733151690498049");
    BigNum z("466689128243102636255574657362541798675");
    std::cout << t.Mod(m, z) << '\n';
}
