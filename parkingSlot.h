#ifndef PARKINGSLOT_H
#define PARKINGSLOT_H

class ParkingSlot {
public:
    int slotId;
    bool isFree;

    ParkingSlot* next;
    ParkingSlot* prev;

    ParkingSlot(int id) {
        slotId = id;
        isFree = true;
        next = prev = nullptr;
    }
};

#endif
