#include "Interactable.hpp"
// #include <iostream>

Interactable::Interactable(int id) : GameObject(id) {}

void Interactable::interact() {
    // std::cout << "Interactable " << id << " triggered\n";
}

bool Interactable::isEnabled() const {
    return enabled;
}
