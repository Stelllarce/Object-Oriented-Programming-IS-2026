#pragma once
#include "engine.hpp"  // Full include – Car contains Engine by value

class Car {
    Engine engine;
    const char* model;
public:
    Car(const char* m, int hp);

    const char* getModel() const;
    Engine& getEngine();
};
