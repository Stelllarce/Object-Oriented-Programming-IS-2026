#include "roster.hpp"
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
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return;
    }
    out << r.name << " " << r.count << "\n";
    for (int i = 0; i < r.count; i++) {
        write(out, r.heroes[i]);
    }
    out.close();
    if (out.fail()) {
        std::cerr << "Write error: " << filename << "\n";
    }
}

void loadFromFile(Roster& r, const char* filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return;
    }
    char name[64];
    int count;
    in >> name >> count;
    init(r, name);
    for (int i = 0; i < count; i++) {
        Hero h;
        read(in, h);
        addHero(r, h);
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
