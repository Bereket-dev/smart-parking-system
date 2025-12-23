#ifndef PARKING_SYSTEM_H
#define PARKING_SYSTEM_H

#include "parkingSlot.h"
#include "Queue.h"
#include <string>

class ParkingSystem {
private:
    // Constants for slot allocation - 20 TOTAL SLOTS
    static const int TOTAL_SLOTS = 20;
    static const int EMERGENCY_SLOTS = 3;
    static const int VIP_SLOTS = 7;
    static const int NORMAL_SLOTS = 10;
    
    // Slot ranges for 20 slots
    static const int EMERGENCY_START = 0;
    static const int EMERGENCY_END = 2;     // Slots 0-2 = Emergency (3 slots)
    static const int VIP_START = 3;
    static const int VIP_END = 9;           // Slots 3-9 = VIP (7 slots)
    static const int NORMAL_START = 10;
    static const int NORMAL_END = 19;       // Slots 10-19 = Normal (10 slots)
    
    ParkingSlot slots[TOTAL_SLOTS];
    Queue normalQueue;
    Queue vipQueue;
    Queue emergencyQueue;
    Queue emergencyWaitingQueue;
    
    // Helper methods
    int findEmptySlotForPriority(int priority);
    bool handleEmergencyFull(Vehicle& vehicle);
    int findNearestSlotLinear(int preferredSlot);
    int calculateDistance(int slot1, int slot2);
    
public:
    // Constructor
    ParkingSystem();
    
    // Core parking operations
    void parkVehicle();
    void removeVehicle();
    void searchVehicle();
    void findNearestFreeSlot();
    void displayParkingStatus();
    void displayWaitingQueues();
    
    // Utility methods
    int getFreeSlotsCount() const;
    int getOccupiedSlotsCount() const;
    bool isParkingFull() const;
    int getFreeSlotsForPriority(int priority) const;
    
    // Priority-specific methods
    void displayEmergencySlots() const;
    void displayVipSlots() const;
    void displayNormalSlots() const;
};

#endif