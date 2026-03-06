#pragma once

#include <fstream>

struct Hero {
    char* name; // single word, e.g. "Aragorn"
    int health;
    int attack;
};

void init(Hero& h, const char* name, int health, int attack);
void destroy(Hero& h);

void write(std::ofstream& out, const Hero& h);
void read(std::ifstream& in, Hero& h);
