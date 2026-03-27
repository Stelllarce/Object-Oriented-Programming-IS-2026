#pragma once

class Car;  // Forward declaration – avoids circular include

class Engine {
    int horsepower;
    Car* owner;  // Only a pointer – forward declaration is enough
public:
    Engine(int hp);

    void setOwner(Car* c);
    int getHorsepower() const;
};
