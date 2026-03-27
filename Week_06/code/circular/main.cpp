#include <iostream>
#include "car.hpp"

int main() {
    Car car("Toyota", 150);
    // Output: Engine (150hp) assigned to Toyota

    std::cout << "Car: " << car.getModel() << std::endl;
    std::cout << "Engine: " << car.getEngine().getHorsepower() << "hp" << std::endl;

    return 0;
}

// Compile: g++ main.cpp car.cpp engine.cpp -o circular
