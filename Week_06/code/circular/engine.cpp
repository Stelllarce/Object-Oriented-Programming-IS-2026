#include "engine.hpp"
#include "car.hpp"  // Full include needed here to use Car's methods

#include <iostream>

Engine::Engine(int hp) : horsepower(hp), owner(nullptr) {}

void Engine::setOwner(Car* c) {
    owner = c;
    if (owner) {
        std::cout << "Engine (" << horsepower << "hp) assigned to " << owner->getModel() << std::endl;
    }
}

int Engine::getHorsepower() const { return horsepower; }
