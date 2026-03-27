#include <iostream>

// Demonstrates ownership semantics with member data

// ---- By value: full ownership ----
class Engine {
    int horsepower;
public:
    Engine(int hp) : horsepower(hp) {
        std::cout << "Engine(" << hp << ") created" << std::endl;
    }
    ~Engine() {
        std::cout << "Engine(" << horsepower << ") destroyed" << std::endl;
    }
    int getHorsepower() const { return horsepower; }
};

class Car {
    Engine engine;  // Car owns the Engine (by value)
public:
    Car(int hp) : engine(hp) {}
    // Engine is automatically destroyed when Car is destroyed
};

// ---- Owning pointer: class manages the resource ----
class Team {
    char* name;  // Owning pointer – Team must free this
public:
    Team(const char* n) {
        size_t len = 0;
        while (n[len]) len++;
        name = new char[len + 1];
        for (size_t i = 0; i <= len; i++) name[i] = n[i];
        std::cout << "Team \"" << name << "\" created" << std::endl;
    }
    ~Team() {
        std::cout << "Team \"" << name << "\" destroyed" << std::endl;
        delete[] name;  // Must free because we own it
    }

    // Prevent copying
    Team(const Team&) = delete;
    Team& operator=(const Team&) = delete;
};

// ---- Reference: no ownership ----
class Logger {
    std::ostream& output;  // Non-owning reference
public:
    Logger(std::ostream& os) : output(os) {}

    void log(const char* msg) {
        output << "[LOG] " << msg << std::endl;
    }
    // No destructor needed – Logger doesn't own the stream
};

int main() {
    std::cout << "=== By value (ownership) ===" << std::endl;
    {
        Car car(200);
    }  // Car destroyed → Engine destroyed automatically

    std::cout << "\n=== Owning pointer ===" << std::endl;
    {
        Team team("Rockets");
    }  // Team destroyed → name freed in destructor

    std::cout << "\n=== Reference (no ownership) ===" << std::endl;
    {
        Logger logger(std::cout);
        logger.log("Hello from logger");
    }  // logger destroyed, but std::cout lives on

    std::cout << "std::cout still works after Logger is gone" << std::endl;

    return 0;
}

// Compile: g++ main.cpp -o ownership
