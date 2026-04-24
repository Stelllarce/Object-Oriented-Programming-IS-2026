#pragma once

#include "hero.hpp"

const int MAX_HEROES = 16;

struct Roster {
    char* name; // single word, e.g. "Fellowship"
    Hero heroes[MAX_HEROES];
    int count;
};

void init(Roster& r, const char* name);
void destroy(Roster& r);
void addHero(Roster& r, const Hero& h);

// File format:
//   name count\n
//   hero1_name hero1_health hero1_attack\n
//   hero2_name ...
void saveToFile(const Roster& r, const char* filename);
void loadFromFile(Roster& r, const char* filename);

void print(const Roster& r);
