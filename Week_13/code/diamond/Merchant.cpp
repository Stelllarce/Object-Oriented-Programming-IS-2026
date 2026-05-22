#include "Merchant.hpp"
// #include <iostream>

Merchant::Merchant(int id, int health, int gold)
    : GameObject(id),        // virtual base: the most-derived class constructs it
      Actor(id, health),
      Interactable(id),
      gold(gold) {}

void Merchant::update(float dt) {
    Actor::update(dt);       // resolve the dominance explicitly
}

void Merchant::interact() {
    // std::cout << "Merchant " << id << " offers wares (" << gold << " gold)\n";
}

void Merchant::trade(int amount) {
    gold += amount;
}
