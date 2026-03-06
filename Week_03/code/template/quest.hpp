#pragma once

#include <fstream>

// Quest title and description can contain spaces
struct Quest {
    char* title;
    char* description;
    int reward;
    int bonusReward;
};

void init(Quest& q, const char* title, const char* description, int reward, int bonusReward);
void destroy(Quest& q);

void write(std::ofstream& out, const Quest& q);
void read(std::ifstream& in, Quest& q);
