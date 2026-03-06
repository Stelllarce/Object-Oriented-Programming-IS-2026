#include "hero.hpp"
#include <cstring>
#include <iostream>

void init(Hero& h, const char* name, int health, int attack) {
    // TODO: allocate h.name and copy name into it
    // TODO: set health and attack
}

void destroy(Hero& h) {
    // TODO: free h.name and set it to nullptr
}

void write(std::ofstream& out, const Hero& h) {
    // TODO: write in format: name health attack\n
}

void read(std::ifstream& in, Hero& h) {
    // TODO: read name (single word), health and attack
    // Don't forget to allocate memory for h.name
}
