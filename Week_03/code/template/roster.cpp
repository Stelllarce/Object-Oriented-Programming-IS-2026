#include "roster.hpp"
#include <cstring>
#include <fstream>
#include <iostream>

void init(Roster& r, const char* name) {
    // TODO: allocate r.name and copy name into it
    // TODO: set count to 0
}

void destroy(Roster& r) {
    // TODO: free r.name
    // TODO: call destroy() for each hero in heroes[0..count-1]
}

void addHero(Roster& r, const Hero& h) {
    if (r.count >= MAX_HEROES) return;
    // TODO: deep copy h into heroes[count] (allocate new name, copy fields)
    r.count++;
}

void saveToFile(const Roster& r, const char* filename) {
    // TODO: open file for writing, check if open succeeded
    // TODO: write roster name and count: name count\n
    // TODO: write each hero using write()
}

void loadFromFile(Roster& r, const char* filename) {
    // TODO: open file for reading, check if open succeeded
    // TODO: read roster name and count
    // TODO: read each hero using read()
}

void print(const Roster& r) {
    std::cout << "Roster: " << r.name << " (" << r.count << " heroes)\n";
    for (int i = 0; i < r.count; i++) {
        std::cout << "  " << r.heroes[i].name
                  << " HP:" << r.heroes[i].health
                  << " ATK:" << r.heroes[i].attack << "\n";
    }
}
