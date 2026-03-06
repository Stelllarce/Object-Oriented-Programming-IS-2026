#include "quest.hpp"
#include <cstring>

void init(Quest& q, const char* title, const char* description, int reward, int bonusReward) {
    // TODO: allocate and copy title
    // TODO: allocate and copy description
    // TODO: set reward and bonusReward
}

void destroy(Quest& q) {
    // TODO: free q.title and q.description, set both to nullptr
}

void write(std::ofstream& out, const Quest& q) {
    // TODO: choose a file format and write all four fields
    // Both title and description can contain spaces - keep that in mind when designing the format
}

void read(std::ifstream& in, Quest& q) {
    // TODO: read all four fields using the format you chose in write()
    // Remember: title and description contain spaces, so >> alone won't work for them
}
