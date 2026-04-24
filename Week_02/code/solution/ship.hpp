#pragma once

#include "config.h"

struct ShipStatus {
    unsigned char engines   : 1;
    unsigned char shields   : 1;
    unsigned char weapons   : 1;
    unsigned char docked    : 1;
    unsigned char fuelLevel : 3;
};

namespace Fleet {

    enum ShipType {
        Fighter = 0,
        Cruiser = 1,
        Destroyer = 2
    };

    const char* typeToString(ShipType type);

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

namespace Civilian {

    enum ShipType {
        Transport = 0,
        Shuttle = 1,
        Mining = 2
    };

    const char* typeToString(ShipType type);

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
