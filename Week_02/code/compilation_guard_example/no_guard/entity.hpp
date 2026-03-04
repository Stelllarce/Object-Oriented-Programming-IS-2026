#include "vector.hpp"
// Effectively expands to:
// struct Vector2D {
//     float x;
//     float y;
// };

struct Entity {
    int id;
    Vector2D position; // Crucial: Needs Vector2D to define an entity
};