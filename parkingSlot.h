#ifndef PARKINGSLOT_H
#define PARKINGSLOT_H

#include "Vehicle.h"

struct ParkingSlot {
    int id;
    bool occupied;
    Vehicle vehicle;
};

#endif
