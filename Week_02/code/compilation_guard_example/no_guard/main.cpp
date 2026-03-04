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
// Effectively expands to:
// struct Vector2D {
//     float x;
//     float y;
// };
// void applyGravity(Vector2D& pos) {
//     pos.y -= 9.8f; 
// }
// We now have two definitions of Vector2D in the same translation unit, which causes a redefinition error during compilation.

int main() {
    Entity player;
    player.position.x = 0.0f;
    player.position.y = 100.0f;

    // We need both headers included in main to do this:
    applyGravity(player.position); 

    return 0;
}