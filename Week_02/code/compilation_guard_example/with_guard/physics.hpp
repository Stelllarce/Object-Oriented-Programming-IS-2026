
#pragma once
#include "vector.hpp"
// Effectively expands to:
// struct Vector2D {
//     float x;
//     float y;
// };

// Needs Vector2D to know how to apply gravity to a position
void applyGravity(Vector2D& pos) {
    pos.y -= 9.8f; 
}