#include "quest.hpp"
#include <cstring>

void init(Quest& q, const char* title, const char* description, int reward, int bonusReward) {
    q.title = new char[strlen(title) + 1];
    strcpy(q.title, title);
    q.description = new char[strlen(description) + 1];
    strcpy(q.description, description);
    q.reward = reward;
    q.bonusReward = bonusReward;
}

void destroy(Quest& q) {
    delete[] q.title;
    q.title = nullptr;
    delete[] q.description;
    q.description = nullptr;
}

void write(std::ofstream& out, const Quest& q) {
    out << q.title << "\n"
        << q.description << "\n"
        << q.reward << "\n"
        << q.bonusReward << "\n";
}

void read(std::ifstream& in, Quest& q) {
    char buf[256];
    in.getline(buf, sizeof(buf));
    q.title = new char[strlen(buf) + 1];
    strcpy(q.title, buf);
    in.getline(buf, sizeof(buf));
    q.description = new char[strlen(buf) + 1];
    strcpy(q.description, buf);
    in >> q.reward >> q.bonusReward;
    in.ignore();
}
