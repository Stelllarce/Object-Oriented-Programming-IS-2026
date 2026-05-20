#include "Overdrive.hpp"

Overdrive* Overdrive::clone() {
    return new Overdrive(*this);
}