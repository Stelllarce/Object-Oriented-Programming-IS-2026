#pragma once
#include "Actor.hpp"
#include "Interactable.hpp"

class Merchant : public Actor, public Interactable {
public:
    Merchant(int id, int health, int gold);

    void update(float dt) override;
    void interact() override;
    void trade(int amount);

private:
    int gold;
};
