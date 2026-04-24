#include "roster.hpp"
#include "quest.hpp"
#include <iostream>

int main() {
    // --- Part 1: Heroes and Roster ---
    Hero h1, h2, h3;
    init(h1, "Aragorn", 120, 30);
    init(h2, "Legolas", 90,  45);
    init(h3, "Gimli",   150, 25);

    Roster r;
    init(r, "Fellowship");
    addHero(r, h1);
    addHero(r, h2);
    addHero(r, h3);

    saveToFile(r, "roster.bin");
    print(r);

    Roster loaded;
    loadFromFile(loaded, "roster.bin");
    std::cout << "\nLoaded from file:\n";
    print(loaded);

    // --- Part 2: Quests (titles with spaces) ---
    Quest q1, q2;
    init(q1, "Find the Lost Crown", "An ancient relic hidden in the ruins.", 500, 100);
    init(q2, "Slay the Cave Troll",  "The troll has terrorized the village for weeks.", 200, -1);

    std::ofstream qout("quests.bin", std::ios::binary);
    write(qout, q1);
    write(qout, q2);
    qout.close();
    if (qout.fail()) {
        std::cerr << "Write error: quests.bin\n";
        return 1;
    }

    Quest lq1, lq2;
    std::ifstream qin("quests.bin", std::ios::binary);
    read(qin, lq1);
    read(qin, lq2);
    qin.close();
    if (qin.fail()) {
        std::cerr << "Read error: quests.bin\n";
        return 1;
    }

    std::cout << "\nQuests:\n";
    std::cout << lq1.title << " - " << lq1.description << " - " << lq1.reward << " gold";
    if (lq1.bonusReward != -1) std::cout << " (bonus: " << lq1.bonusReward << ")";
    std::cout << "\n";
    std::cout << lq2.title << " - " << lq2.description << " - " << lq2.reward << " gold";
    if (lq2.bonusReward != -1) std::cout << " (bonus: " << lq2.bonusReward << ")";
    std::cout << "\n";

    destroy(h1); destroy(h2); destroy(h3);
    destroy(r);  destroy(loaded);
    destroy(q1); destroy(q2);
    destroy(lq1); destroy(lq2);

    return 0;
}
