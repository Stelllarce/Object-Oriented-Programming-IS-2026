#include "hero.hpp"
#include <cstddef>
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
    const std::size_t nameLen = std::strlen(h.name);
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    out.write(h.name, static_cast<std::streamsize>(nameLen));
    out.write(reinterpret_cast<const char*>(&h.health), sizeof(h.health));
    out.write(reinterpret_cast<const char*>(&h.attack), sizeof(h.attack));
}

void read(std::ifstream& in, Hero& h) {
    std::size_t nameLen = 0;
    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    if (!in) return;

    h.name = new char[nameLen + 1];
    in.read(h.name, static_cast<std::streamsize>(nameLen));
    if (!in) {
        delete[] h.name;
        h.name = nullptr;
        return;
    }
    h.name[nameLen] = '\0';

    in.read(reinterpret_cast<char*>(&h.health), sizeof(h.health));
    in.read(reinterpret_cast<char*>(&h.attack), sizeof(h.attack));
}
