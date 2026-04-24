#include "roster.hpp"
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>

void init(Roster& r, const char* name) {
    r.name = new char[strlen(name) + 1];
    strcpy(r.name, name);
    r.count = 0;
}

void destroy(Roster& r) {
    delete[] r.name;
    r.name = nullptr;
    for (int i = 0; i < r.count; i++) {
        destroy(r.heroes[i]);
    }
    r.count = 0;
}

void addHero(Roster& r, const Hero& h) {
    if (r.count >= MAX_HEROES) return;
    init(r.heroes[r.count], h.name, h.health, h.attack);
    r.count++;
}

void saveToFile(const Roster& r, const char* filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return;
    }

    const std::size_t nameLen = std::strlen(r.name);
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    out.write(r.name, static_cast<std::streamsize>(nameLen));
    out.write(reinterpret_cast<const char*>(&r.count), sizeof(r.count));

    for (int i = 0; i < r.count; i++) {
        write(out, r.heroes[i]);
    }

    out.close();
    if (out.fail()) {
        std::cerr << "Write error: " << filename << "\n";
    }
}

void loadFromFile(Roster& r, const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return;
    }

    std::size_t nameLen = 0;
    int count = 0;
    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    if (!in) {
        in.close();
        std::cerr << "Read error: " << filename << "\n";
        return;
    }

    char* name = new char[nameLen + 1];
    in.read(name, static_cast<std::streamsize>(nameLen));
    if (!in) {
        delete[] name;
        in.close();
        std::cerr << "Read error: " << filename << "\n";
        return;
    }
    name[nameLen] = '\0';

    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (!in || count < 0) {
        delete[] name;
        in.close();
        std::cerr << "Read error: " << filename << "\n";
        return;
    }

    init(r, name);
    delete[] name;

    for (int i = 0; i < count; i++) {
        Hero h;
        read(in, h);
        if (!in) {
            break;
        }

        if (r.count < MAX_HEROES) {
            addHero(r, h);
        }
        destroy(h);
    }

    in.close();
    if (in.fail()) {
        std::cerr << "Read error: " << filename << "\n";
    }
}

void print(const Roster& r) {
    std::cout << "Roster: " << r.name << " (" << r.count << " heroes)\n";
    for (int i = 0; i < r.count; i++) {
        std::cout << "  " << r.heroes[i].name
                  << " HP:" << r.heroes[i].health
                  << " ATK:" << r.heroes[i].attack << "\n";
    }
}
