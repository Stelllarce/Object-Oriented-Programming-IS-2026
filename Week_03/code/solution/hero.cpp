#include "hero.hpp"
#include <cstring>

void init(Hero& h, const char* name, int health, int attack) {
    h.name = new char[strlen(name) + 1];
    strcpy(h.name, name);
    h.health = health;
    h.attack = attack;
}

void destroy(Hero& h) {
    delete[] h.name;
    h.name = nullptr;
}

void write(std::ofstream& out, const Hero& h) {
    out << h.name << " " << h.health << " " << h.attack << "\n";
}

void read(std::ifstream& in, Hero& h) {
    char buf[64];
    in >> buf >> h.health >> h.attack;
    h.name = new char[strlen(buf) + 1];
    strcpy(h.name, buf);
}
