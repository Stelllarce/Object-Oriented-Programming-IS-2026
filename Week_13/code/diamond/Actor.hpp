#pragma once
#include "GameObject.hpp"

class Actor : public virtual GameObject {
public:
    Actor(int id, int health);

    void update(float dt) override;
    const char* tag() const override;
    virtual void move(float dx, float dy);

protected:
    int health;
    float speed = 1.0f;
};
