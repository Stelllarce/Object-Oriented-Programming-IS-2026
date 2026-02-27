#include "point.hpp"
#include <iostream>


int main() {
    Point p;
    p.setX(1.0);
    p.setY(2.0);

    std::cout << "Point coordinates: (" << p.getX() << ", " << p.getY() << ")\n";
    return 0;
}