#include <iostream>
#include <cstring>
#include "ship.hpp"

int main() {
    std::cout << "=== Spaceship Registry Tests ===\n\n";

    // sizeof проверки
    std::cout << "sizeof(ShipStatus)    = " << sizeof(ShipStatus) << "\n";
    std::cout << "sizeof(Fleet::Ship)   = " << sizeof(Fleet::Ship) << "\n";
    std::cout << "sizeof(Civilian::Ship) = " << sizeof(Civilian::Ship) << "\n\n";

    // --- Fleet::Ship ---
    std::cout << "--- Fleet Ship ---\n";
    Fleet::Ship warship;
    warship.init("Executor", Fleet::ShipType::Destroyer, 3000);
    warship.print();
    std::cout << "Type: " << Fleet::typeToString(warship.getType()) << "\n";

    // Промяна на статус
    warship.setFuelLevel(5);
    warship.setEngines(true);
    warship.setShields(true);
    warship.print();

    // Невалидни стойности - не трябва да променят нищо
    warship.setFuelLevel(10);
    std::cout << "Fuel after invalid set (10): " << (int)warship.getFuelLevel() << " (expected 5)\n";

    warship.setCrewCount(-5);
    std::cout << "Crew after invalid set (-5): " << warship.getCrewCount() << " (expected 3000)\n";

    // Акостиране
    warship.setDocked(true);
    warship.setEngines(false);
    warship.print();
    std::cout << "\n";

    // --- Civilian::Ship ---
    std::cout << "--- Civilian Ship ---\n";
    Civilian::Ship freighter;
    freighter.init("Millennium", Civilian::ShipType::Transport, 500);
    freighter.print();
    std::cout << "Type: " << Civilian::typeToString(freighter.getType()) << "\n";

    freighter.setFuelLevel(7);
    freighter.setEngines(true);
    freighter.print();

    // Невалидни стойности
    freighter.setCargoCapacity(-100);
    std::cout << "Cargo after invalid set (-100): " << freighter.getCargoCapacity() << " (expected 500)\n";

    freighter.setDocked(true);
    freighter.setEngines(false);
    freighter.print();
    std::cout << "\n";

    // --- Namespace тест ---
    std::cout << "--- Namespace Test ---\n";
    std::cout << "Fleet ship:    " << warship.getName() << " (" << Fleet::typeToString(warship.getType()) << ")\n";
    std::cout << "Civilian ship: " << freighter.getName() << " (" << Civilian::typeToString(freighter.getType()) << ")\n";

    // Fleet::ShipType и Civilian::ShipType са различни типове в различни namespaces
    // Fleet::ShipType t = Civilian::ShipType::Transport; // Грешка при компилация!

    return 0;
}
