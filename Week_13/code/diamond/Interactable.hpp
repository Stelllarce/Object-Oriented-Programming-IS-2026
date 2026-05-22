#pragma once
#include "GameObject.hpp"

class Interactable : public virtual GameObject {
public:
    Interactable(int id);

    virtual void interact();
    bool isEnabled() const;

protected:
    bool enabled = true;
    int interactionRadius = 2;
};
