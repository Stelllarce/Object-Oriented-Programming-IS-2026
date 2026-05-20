#include "Overdrive.hpp"
#include <iostream>

int main() {
    Effect** e = new Effect*[20];
    e[0] = new Overdrive();
    Overdrive* o = dynamic_cast<Overdrive*>(e[0])->clone();
    return 0;
}