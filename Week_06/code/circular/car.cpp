#include "car.hpp"

Car::Car(const char* m, int hp) : engine(hp), model(m) {
    engine.setOwner(this);  // Pass pointer to this Car
}

const char* Car::getModel() const { return model; }

Engine& Car::getEngine() { return engine; }
