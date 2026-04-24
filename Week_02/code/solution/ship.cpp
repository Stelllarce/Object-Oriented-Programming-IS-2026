#include <iostream>
#include <cstring>
#include "ship.hpp"

// ==================== Fleet ====================

const char* Fleet::typeToString(Fleet::ShipType type) {
    switch (type) {
        case Fleet::Fighter:   return "Fighter";
        case Fleet::Cruiser:   return "Cruiser";
        case Fleet::Destroyer: return "Destroyer";
        default:               return "Unknown";
    }
}

// ==================== Fleet::Ship ====================

void Fleet::Ship::init(const char* shipName, Fleet::ShipType shipType, int crew) {
    strncpy(name, shipName, MAX_NAME_LEN - 1);
    name[MAX_NAME_LEN - 1] = '\0';
    type = shipType;
    status.engines = 0;
    status.shields = 0;
    status.weapons = 0;
    status.docked = 0;
    status.fuelLevel = 0;
    crewCount = (crew < 0) ? 0 : crew;
}

const char* Fleet::Ship::getName() {
    return name;
}

Fleet::ShipType Fleet::Ship::getType() {
    return type;
}

int Fleet::Ship::getCrewCount() {
    return crewCount;
}

void Fleet::Ship::setCrewCount(int count) {
    if (count > 0) {
        crewCount = count;
    }
}

unsigned char Fleet::Ship::getFuelLevel() {
    return status.fuelLevel;
}

void Fleet::Ship::setFuelLevel(unsigned char level) {
    if (level <= MAX_FUEL) {
        status.fuelLevel = level;
    }
}

void Fleet::Ship::setEngines(bool on) {
    status.engines = on;
}

void Fleet::Ship::setShields(bool on) {
    status.shields = on;
}

void Fleet::Ship::setDocked(bool state) {
    status.docked = state;
}

void Fleet::Ship::print() {
    std::cout << "[Fleet] " << name
              << " (" << Fleet::typeToString(type) << ")"
              << " | Crew: " << crewCount
              << " | Fuel: " << (int)status.fuelLevel << "/7"
              << " | Engines: " << (status.engines ? "on" : "off")
              << " | Shields: " << (status.shields ? "on" : "off")
              << " | Docked: " << (status.docked ? "yes" : "no")
              << "\n";
}

// ==================== Civilian ====================

const char* Civilian::typeToString(Civilian::ShipType type) {
    switch (type) {
        case Civilian::Transport: return "Transport";
        case Civilian::Shuttle:   return "Shuttle";
        case Civilian::Mining:    return "Mining";
        default:                  return "Unknown";
    }
}

// ==================== Civilian::Ship ====================

void Civilian::Ship::init(const char* shipName, Civilian::ShipType shipType, int cargo) {
    strncpy(name, shipName, MAX_NAME_LEN - 1);
    name[MAX_NAME_LEN - 1] = '\0';
    type = shipType;
    status.engines = 0;
    status.shields = 0;
    status.weapons = 0;
    status.docked = 0;
    status.fuelLevel = 0;
    cargoCapacity = (cargo < 0) ? 0 : cargo;
}

const char* Civilian::Ship::getName() {
    return name;
}

Civilian::ShipType Civilian::Ship::getType() {
    return type;
}

int Civilian::Ship::getCargoCapacity() {
    return cargoCapacity;
}

void Civilian::Ship::setCargoCapacity(int capacity) {
    if (capacity >= 0) {
        cargoCapacity = capacity;
    }
}

unsigned char Civilian::Ship::getFuelLevel() {
    return status.fuelLevel;
}

void Civilian::Ship::setFuelLevel(unsigned char level) {
    if (level <= MAX_FUEL) {
        status.fuelLevel = level;
    }
}

void Civilian::Ship::setEngines(bool on) {
    status.engines = on;
}

void Civilian::Ship::setDocked(bool state) {
    status.docked = state;
}

void Civilian::Ship::print() {
    std::cout << "[Civilian] " << name
              << " (" << Civilian::typeToString(type) << ")"
              << " | Cargo: " << cargoCapacity
              << " | Fuel: " << (int)status.fuelLevel << "/7"
              << " | Engines: " << (status.engines ? "on" : "off")
              << " | Docked: " << (status.docked ? "yes" : "no")
              << "\n";
}
