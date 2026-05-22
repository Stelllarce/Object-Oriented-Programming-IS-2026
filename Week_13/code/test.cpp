#include <iostream>
class Vehicle {
public:
    int id = 0;
    Vehicle(int id) : id(id) { std::cout << "Vehicle constructor\n"; }
};

class Tank : virtual public Vehicle { public: Tank(int id) : Vehicle(id) { std::cout << "Tank constructor\n";}};
class Boat : virtual public Vehicle { public: Boat(int id) : Vehicle(id) { std::cout << "Boat constructor\n";}};

class Amphybian : public Tank, public Boat {
public:
    Amphybian(int id) : Vehicle(id), Tank(id), Boat(id) { std::cout << "Amphybian constructor\n";}
};

struct Takovato : public Amphybian {
    Takovato(int id) : Vehicle(id), Amphybian(id) { std::cout << "Takovato constructor\n";}
};

int main(int argc, char const *argv[])
{
    Takovato t(20);

    // std::cout << b.id << " " << a.id;

    return 0;
}
