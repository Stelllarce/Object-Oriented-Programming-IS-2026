#include <iostream>
#include <cstring>
#include "ship.hpp"

// ==================== Fleet ====================

const char* Fleet::typeToString(Fleet::ShipType type) {
    // TODO: Върнете "Fighter", "Cruiser" или "Destroyer" спрямо type
    // Ако стойността е невалидна, върнете "Unknown"
    return nullptr;
}

// ==================== Fleet::Ship ====================

void Fleet::Ship::init(const char* shipName, Fleet::ShipType shipType, int crew) {
    // TODO: Копирайте shipName в name (strncpy)
    // TODO: Задайте type
    // TODO: Занулете всички полета на status
    // TODO: Задайте crewCount (ако crew < 0, задайте 0)
}

const char* Fleet::Ship::getName() {
    // TODO
    return nullptr;
}

Fleet::ShipType Fleet::Ship::getType() {
    // TODO
    return Fleet::ShipType::Fighter;
}

int Fleet::Ship::getCrewCount() {
    // TODO
    return 0;
}

void Fleet::Ship::setCrewCount(int count) {
    // TODO: Задайте само ако count > 0
}

unsigned char Fleet::Ship::getFuelLevel() {
    // TODO
    return 0;
}

void Fleet::Ship::setFuelLevel(unsigned char level) {
    // TODO: Задайте само ако level <= MAX_FUEL
}

void Fleet::Ship::setEngines(bool on) {
    // TODO
}

void Fleet::Ship::setShields(bool on) {
    // TODO
}

void Fleet::Ship::setDocked(bool state) {
    // TODO
}

void Fleet::Ship::print() {
    // TODO: Изведете:
    // [Fleet] <name> (<type>) | Crew: <crewCount> | Fuel: <fuelLevel>/7 | Engines: on/off | Shields: on/off | Docked: yes/no
    // Използвайте typeToString за извеждане на типа
}

// ==================== Civilian ====================

const char* Civilian::typeToString(Civilian::ShipType type) {
    // TODO: Върнете "Transport", "Shuttle" или "Mining" спрямо type
    // Ако стойността е невалидна, върнете "Unknown"
    return nullptr;
}

// ==================== Civilian::Ship ====================

void Civilian::Ship::init(const char* shipName, Civilian::ShipType shipType, int cargo) {
    // TODO: Копирайте shipName в name (strncpy)
    // TODO: Задайте type
    // TODO: Занулете всички полета на status
    // TODO: Задайте cargoCapacity (ако cargo < 0, задайте 0)
}

const char* Civilian::Ship::getName() {
    // TODO
    return nullptr;
}

Civilian::ShipType Civilian::Ship::getType() {
    // TODO
    return Civilian::ShipType::Transport;
}

int Civilian::Ship::getCargoCapacity() {
    // TODO
    return 0;
}

void Civilian::Ship::setCargoCapacity(int capacity) {
    // TODO: Задайте само ако capacity >= 0
}

unsigned char Civilian::Ship::getFuelLevel() {
    // TODO
    return 0;
}

void Civilian::Ship::setFuelLevel(unsigned char level) {
    // TODO: Задайте само ако level <= MAX_FUEL
}

void Civilian::Ship::setEngines(bool on) {
    // TODO
}

void Civilian::Ship::setDocked(bool state) {
    // TODO
}

void Civilian::Ship::print() {
    // TODO: Изведете:
    // [Civilian] <name> (<type>) | Cargo: <cargoCapacity> | Fuel: <fuelLevel>/7 | Engines: on/off | Docked: yes/no
    // Използвайте typeToString за извеждане на типа
}
