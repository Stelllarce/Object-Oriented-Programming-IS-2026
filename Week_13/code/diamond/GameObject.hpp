#pragma once

class GameObject {
public:
    GameObject(int id);
    virtual ~GameObject();

    virtual void update(float dt);
    virtual const char* tag() const;

    int getId() const;

protected:
    int id;
    float x = 0.0f;
    float y = 0.0f;
};
