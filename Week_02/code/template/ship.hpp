#pragma once

#include "config.h"

// Статус на кораб чрез битови полета
// Използвайте unsigned char за минимален размер
struct ShipStatus {
    // TODO: Дефинирайте следните битови полета:
    // engines   - 1 бит (0 = изключени, 1 = включени)
    // shields   - 1 бит (0 = свалени, 1 = вдигнати)
    // weapons   - 1 бит (0 = неактивни, 1 = активни)
    // docked    - 1 бит (0 = в полет, 1 = акостирал)
    // fuelLevel - 3 бита (стойности 0–7)
};

// Военен флот
namespace Fleet {

    // TODO: Дефинирайте enum ShipType със стойности:
    // Fighter = 0, Cruiser = 1, Destroyer = 2

    // TODO: Имплементирайте функция, която преобразува ShipType към текст
    // const char* typeToString(ShipType type);

    struct Ship {
    private:
        char name[MAX_NAME_LEN];
        ShipType type;
        ShipStatus status;
        int crewCount;

    public:
        void init(const char* shipName, ShipType shipType, int crew);

        const char* getName();
        ShipType getType();
        int getCrewCount();
        void setCrewCount(int count);

        unsigned char getFuelLevel();
        void setFuelLevel(unsigned char level);

        void setEngines(bool on);
        void setShields(bool on);
        void setDocked(bool state);

        void print();
    };

}

// Граждански кораби
namespace Civilian {

    // TODO: Дефинирайте enum ShipType със стойности:
    // Transport = 0, Shuttle = 1, Mining = 2

    // TODO: Имплементирайте функция, която преобразува ShipType към текст
    // const char* typeToString(ShipType type);

    struct Ship {
    private:
        char name[MAX_NAME_LEN];
        ShipType type;
        ShipStatus status;
        int cargoCapacity;

    public:
        void init(const char* shipName, ShipType shipType, int cargo);

        const char* getName();
        ShipType getType();
        int getCargoCapacity();
        void setCargoCapacity(int capacity);

        unsigned char getFuelLevel();
        void setFuelLevel(unsigned char level);

        void setEngines(bool on);
        void setDocked(bool state);

        void print();
    };

}
