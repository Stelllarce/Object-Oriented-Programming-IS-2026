#include "Merchant.hpp"
// #include <iostream>

int main() {
    Merchant m(7, 100, 500);

    m.update(0.5f);      // Actor::update dominates over GameObject::update
    m.interact();        // Merchant::interact
    m.move(1.0f, 0.0f);  // inherited from Actor
    m.trade(50);

    // std::cout << "id = " << m.getId() << '\n';  // single shared GameObject – unambiguous

    GameObject* g = &m;  // upcast to the one virtual base subobject
    g->update(1.0f);     // virtual dispatch lands on Merchant::update

    return 0;
}
