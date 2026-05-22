#include "Actor.hpp"
// #include <iostream>

Actor::Actor(int id, int health) : GameObject(id), health(health) {}

void Actor::update(float dt) {
    move(speed * dt, 0.0f);
}

const char* Actor::tag() const {
    return "Actor";
}

void Actor::move(float dx, float dy) {
    x += dx;
    y += dy;
    // std::cout << "Actor " << id << " moves to (" << x << ", " << y << ")\n";
}
