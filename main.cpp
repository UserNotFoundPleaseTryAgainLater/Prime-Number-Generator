#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

class Bitset {
    std::vector<uint64_t> data;

  public:
    Bitset(size_t n, bool val = false)
        : data((n + 63) / 64, val ? ~0ULL : 0ULL) {}
    void set() { std::fill(data.begin(), data.end(), ~0ULL); }
    void reset() { std::fill(data.begin(), data.end(), 0ULL); }
    bool operator[](size_t i) const {
        return (data[i / 64] >> (i % 64)) & 1ULL;
    }
    void set(size_t i, bool val) {
        if (val)
            data[i / 64] |= (1ULL << (i % 64));
        else
            data[i / 64] &= ~(1ULL << (i % 64));
    }
    size_t size() const { return data.size() * 64; }
};

constexpr unsigned short offsets[8] = {1, 7, 11, 13, 17, 19, 23, 29};
constexpr short offsetLookup[31] = {-1, 0,  -1, -1, -1, -1, -1, 1,  -1, -1, -1,
                                    2,  -1, 3,  -1, -1, -1, 4,  -1, 5,  -1, -1,
                                    -1, 6,  -1, -1, -1, -1, -1, 7,  -1};

template <typename T>
void WriteToFile(std::vector<T> &PrimeArray, Bitset &isPrime) {
    std::ofstream out("primes.txt");
    std::string buffer;
    buffer.reserve(1 << 20);
    for (T i = 0; i < PrimeArray.size(); ++i) {
        if (isPrime[i]) {
            buffer += std::to_string(PrimeArray[i]);
            buffer += "\n";
            if (buffer.size() > (1 << 20)) {
                out << buffer;
                buffer.clear();
            }
        }
    }
    if (!buffer.empty())
        out << buffer;
}

template <typename T> std::vector<T> WheelFactorization(T inputSize) {
    std::vector<T> PrimeArray;
    if (inputSize >= 2)
        PrimeArray.push_back(2);
    if (inputSize >= 3)
        PrimeArray.push_back(3);
    if (inputSize >= 5)
        PrimeArray.push_back(5);
    for (T iteration = 0; iteration <= inputSize / 30; iteration++) {
        for (const auto offset : offsets) {
            T val = iteration * 30 + offset;
            if (val <= 5)
                continue;
            if (val > inputSize)
                break;
            PrimeArray.push_back(val);
        }
    }
    return PrimeArray;
}

template <typename T> void ClassicSieve(T input) {
    Bitset isPrime(input + 1, true);
    isPrime.set(0, false);
    isPrime.set(1, false);
    T sieveLimit = static_cast<T>(std::floor(std::sqrt(input)));
    for (T i = 2; i <= sieveLimit; ++i) {
        if (!isPrime[i])
            continue;
        for (T multiple = i * i; multiple <= input; multiple += i) {
            isPrime.set(multiple, false);
        }
    }
    for (T i = 2; i <= input; ++i) {
        if (isPrime[i])
            std::cout << i << " ";
    }
    std::cout << std::endl;
}

template <typename T> void PrimeGeneration(T input) {
    auto start = std::chrono::steady_clock::now();
    if (input < 1000000000) {
        ClassicSieve(input);
    } else {
        std::vector<T> PrimeArray = WheelFactorization(input);
        Bitset isPrime(PrimeArray.size(), true);
        T sieveLimit = static_cast<T>(std::floor(std::sqrt(input)));
        for (T i = 0; i < PrimeArray.size(); ++i) {
            if (!isPrime[i])
                continue;
            T prime = PrimeArray[i];
            if (prime > sieveLimit)
                break;
            for (T j = i + 1; j < PrimeArray.size(); ++j) {
                T num = PrimeArray[j];
                if (num % prime == 0)
                    isPrime.set(j, false);
            }
        }
        if (input <= 1000000) {
            for (T i = 0; i < PrimeArray.size(); ++i) {
                if (isPrime[i])
                    std::cout << PrimeArray[i] << " ";
            }
            std::cout << "\n";
        } else {
            WriteToFile(PrimeArray, isPrime);
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    float timeElapsed = std::round((elapsed.count() / 1000.0f) * 10) / 10.0f;
    std::cout << "Process took " << timeElapsed << " seconds.\n";
}

int main() {
    unsigned long long input = 0;
    std::cout << "Finding primes from 1 to what number? (max is 10 billion) ";
    std::cin >> input;
    if (input < 2) {
        std::cout << "No primes in this range.\n";
        return 0;
    }
    if (input > 10000000000ULL) {
        std::cout << "Input too large.\n";
        return 0;
    }
    if (input <= std::numeric_limits<unsigned short>::max())
        PrimeGeneration<unsigned short>(static_cast<unsigned short>(input));
    else if (input <= std::numeric_limits<unsigned int>::max())
        PrimeGeneration<unsigned int>(static_cast<unsigned int>(input));
    else if (input <= std::numeric_limits<unsigned long>::max())
        PrimeGeneration<unsigned long>(static_cast<unsigned long>(input));
    else
        PrimeGeneration<unsigned long long>(
            static_cast<unsigned long long>(input));
    return 0;
}
