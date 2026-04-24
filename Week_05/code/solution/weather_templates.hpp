#pragma once

#include <cstddef>
#include <fstream>
#include <iostream>

struct Station {
    int id;
    char name[32];
    float latitude;
    float longitude;
};

template <typename T>
struct Measurement {
    int station_id;
    int timestamp;
    T value;
};

template <typename T, std::size_t N>
bool writeToFile(const char* filename, const T (&arr)[N]) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(arr), static_cast<std::streamsize>(sizeof(T) * N));
    return out.good();
}

template <typename T>
bool readFromFile(const char* filename, T* arr, std::size_t count) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) return false;
    in.read(reinterpret_cast<char*>(arr), static_cast<std::streamsize>(sizeof(T) * count));
    return in.gcount() == static_cast<std::streamsize>(sizeof(T) * count);
}

template <typename T>
std::size_t countRecords(const char* filename) {
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    if (!in) return 0;
    std::streamsize bytes = in.tellg();
    if (bytes < 0) return 0;
    return static_cast<std::size_t>(bytes) / sizeof(T);
}

template <typename T, std::size_t N>
const T* findFirst(const T (&arr)[N], bool (*pred)(const T&)) {
    for (std::size_t i = 0; i < N; ++i) {
        if (pred(arr[i])) return &arr[i];
    }
    return nullptr;
}

template <typename T>
void printRecord(const T&) {
    std::cout << "[непознат запис: " << sizeof(T) << " байта]\n";
}

template <>
inline void printRecord<Station>(const Station& s) {
    std::cout << "Станция #" << s.id << " | " << s.name << " | " << s.latitude << ", " << s.longitude << "\n";
}

template <>
inline void printRecord<Measurement<float>>(const Measurement<float>& m) {
    std::cout << "Измерване от станция #" << m.station_id << " @ t=" << m.timestamp << " | стойност: " << m.value << "\n";
}
