#include "quest.hpp"
#include <cstddef>
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
    const std::size_t titleLen = std::strlen(q.title);
    const std::size_t descriptionLen = std::strlen(q.description);

    out.write(reinterpret_cast<const char*>(&titleLen), sizeof(titleLen));
    out.write(q.title, static_cast<std::streamsize>(titleLen));
    out.write(reinterpret_cast<const char*>(&descriptionLen), sizeof(descriptionLen));
    out.write(q.description, static_cast<std::streamsize>(descriptionLen));
    out.write(reinterpret_cast<const char*>(&q.reward), sizeof(q.reward));
    out.write(reinterpret_cast<const char*>(&q.bonusReward), sizeof(q.bonusReward));
}

void read(std::ifstream& in, Quest& q) {
    std::size_t titleLen = 0;
    std::size_t descriptionLen = 0;

    in.read(reinterpret_cast<char*>(&titleLen), sizeof(titleLen));
    if (!in) return;
    q.title = new char[titleLen + 1];
    in.read(q.title, static_cast<std::streamsize>(titleLen));
    if (!in) {
        delete[] q.title;
        q.title = nullptr;
        return;
    }
    q.title[titleLen] = '\0';

    in.read(reinterpret_cast<char*>(&descriptionLen), sizeof(descriptionLen));
    if (!in) {
        delete[] q.title;
        q.title = nullptr;
        return;
    }
    q.description = new char[descriptionLen + 1];
    in.read(q.description, static_cast<std::streamsize>(descriptionLen));
    if (!in) {
        delete[] q.title;
        q.title = nullptr;
        delete[] q.description;
        q.description = nullptr;
        return;
    }
    q.description[descriptionLen] = '\0';

    in.read(reinterpret_cast<char*>(&q.reward), sizeof(q.reward));
    in.read(reinterpret_cast<char*>(&q.bonusReward), sizeof(q.bonusReward));
}
