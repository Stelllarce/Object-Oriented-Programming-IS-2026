#include "GameObject.hpp"
// #include <iostream>

GameObject::GameObject(int id) : id(id) {}

GameObject::~GameObject() = default;

void GameObject::update(float) {
    // std::cout << "GameObject " << id << " sits idle\n";
}

const char* GameObject::tag() const {
    return "GameObject";
}

int GameObject::getId() const {
    return id;
}
