#pragma once

#include <fstream>

// Quest title and description can contain spaces
struct Quest {
    char* title;       // e.g. "Find the Lost Crown"
    char* description; // e.g. "An ancient relic hidden in the ruins."
    int reward;
    int bonusReward;   // -1 means no bonus
};

void init(Quest& q, const char* title, const char* description, int reward, int bonusReward);
void destroy(Quest& q);

// File format (one quest per four lines):
//   title\n
//   description\n
//   reward\n
//   bonusReward\n
void write(std::ofstream& out, const Quest& q);
void read(std::ifstream& in, Quest& q);
