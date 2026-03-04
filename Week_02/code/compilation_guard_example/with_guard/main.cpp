#include "entity.hpp"
#include "physics.hpp"
// These expand to:
// ---- entity.hpp ----
// struct Vector2D {
//     float x;
//     float y;
// };
// struct Entity {
//     int id;
//     Vector2D position;
// };
// 
// ---- physics.hpp ----
// void applyGravity(Vector2D& pos) {
//     pos.y -= 9.8f; 
// }
// Preprocessor will skip the contents of vector.hpp the second time because of #pragma once, 
// so we only get one definition of Vector2D, which allows the code to compile successfully.

int main() {
    Entity player;
    player.position.x = 0.0f;
    player.position.y = 100.0f;

    // We need both headers included in main to do this:
    applyGravity(player.position); 

    return 0;
}