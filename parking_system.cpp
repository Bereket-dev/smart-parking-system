#include <iostream>
#include <string>
#include <algorithm>
#include "parking_system.h"
using namespace std;

// Constructor: Initializes all 20 parking slots
// Algorithm:
// 1. Initialize all 20 slots with IDs
// 2. Mark all slots as unoccupied
// Time Complexity: O(n) where n = TOTAL_SLOTS
// Space Complexity: O(1)
ParkingSystem::ParkingSystem() {
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        slots[i].id = i;
        slots[i].occupied = false;
    }
}

// calculateDistance: Calculates distance between two slots
// Algorithm:
// 1. Use absolute difference as distance metric
// 2. Return the absolute difference
// Time Complexity: O(1)
// Space Complexity: O(1)
int ParkingSystem::calculateDistance(int slot1, int slot2) {
    return abs(slot1 - slot2);
}

// findNearestSlotLinear: Finds nearest free slot using linear search
// Algorithm:
// 1. Start from preferred slot (entrance at slot 0)
// 2. Search in expanding radius around preferred slot
// 3. Check slots at increasing distances
// 4. Return first free slot found
// Time Complexity: O(n²) worst case, O(1) best case
// Space Complexity: O(1)
int ParkingSystem::findNearestSlotLinear(int preferredSlot) {
    // Check the preferred slot first (entrance at slot 0)
    if (preferredSlot < TOTAL_SLOTS && !slots[preferredSlot].occupied) {
        return preferredSlot;
    }
    
    // Search in expanding radius around preferred slot
    int maxDistance = max(preferredSlot, TOTAL_SLOTS - 1 - preferredSlot);
    
    for (int distance = 1; distance <= maxDistance; distance++) {
        // Check slot to the right
        int rightSlot = preferredSlot + distance;
        if (rightSlot < TOTAL_SLOTS && !slots[rightSlot].occupied) {
            return rightSlot;
        }
        
        // Check slot to the left
        int leftSlot = preferredSlot - distance;
        if (leftSlot >= 0 && !slots[leftSlot].occupied) {
            return leftSlot;
        }
    }
    
    return -1; // No free slot found
}

// findEmptySlotForPriority: Finds empty slot in priority-specific area
// Algorithm:
// 1. Determine slot range based on priority
// 2. Linear search for first empty slot in range
// 3. Return slot ID or -1 if none found
// Time Complexity: O(n) where n = slots in priority range
// Space Complexity: O(1)
int ParkingSystem::findEmptySlotForPriority(int priority) {
    int startSlot, endSlot;
    
    // Priority system: 1=Normal, 2=VIP, 3=Emergency
    switch (priority) {
        case 3: // Emergency (highest priority)
            startSlot = EMERGENCY_START;
            endSlot = EMERGENCY_END;
            break;
        case 2: // VIP
            startSlot = VIP_START;
            endSlot = VIP_END;
            break;
        case 1: // Normal
            startSlot = NORMAL_START;
            endSlot = NORMAL_END;
            break;
        default:
            return -1;
    }
    
    for (int i = startSlot; i <= endSlot; i++) {
        if (!slots[i].occupied) {
            return i;
        }
    }
    return -1;
}

// parkVehicle: Parks a vehicle based on priority
// Algorithm:
// 1. Get vehicle details from user
// 2. Check for empty slot in priority-specific area
// 3. If found, park vehicle
// 4. If emergency slots full, handle special case
// 5. Otherwise, add to appropriate waiting queue
// Time Complexity: O(n) for finding empty slot
// Space Complexity: O(1)
void ParkingSystem::parkVehicle() {
    Vehicle newVehicle;
    
    cout << "\n=== PARK VEHICLE ===\n";
    cout << "Enter license plate number: ";
    cin >> newVehicle.plateNumber;
    cout << "Enter owner name: ";
    cin >> newVehicle.ownerName;
    
    cout << "Enter priority (1-Normal, 2-VIP, 3-Emergency): ";
    cin >> newVehicle.priority;
    
    // Validate priority input
    while (newVehicle.priority < 1 || newVehicle.priority > 3) {
        cout << "Invalid priority! Enter 1, 2, or 3: ";
        cin >> newVehicle.priority;
    }
    
    int emptySlot = findEmptySlotForPriority(newVehicle.priority);
    
    if (emptySlot != -1) {
        // Found empty slot in priority area
        slots[emptySlot].vehicle = newVehicle;
        slots[emptySlot].occupied = true;
        cout << "✓ Vehicle parked at slot " << emptySlot << "\n";
        cout << "  Priority: ";
        if (newVehicle.priority == 3) cout << "Emergency";
        else if (newVehicle.priority == 2) cout << "VIP";
        else cout << "Normal";
        cout << "\n";
    } else if (newVehicle.priority == 3) {
        // Emergency slots are full - special handling
        bool handled = handleEmergencyFull(newVehicle);
        if (!handled) {
            cout << "✗ Parking failed. No available action taken.\n";
        }
    } else {
        // No slot in priority area, add to waiting queue
        if (newVehicle.priority == 2) {
            vipQueue.enqueue(newVehicle);
            cout << "⚠ VIP slots full. Added to VIP waiting queue.\n";
            cout << "  Queue size: " << vipQueue.getSize() << " vehicles\n";
        } else {
            normalQueue.enqueue(newVehicle);
            cout << "⚠ Normal slots full. Added to normal waiting queue.\n";
            cout << "  Queue size: " << normalQueue.getSize() << " vehicles\n";
        }
    }
}

// handleEmergencyFull: Handles emergency vehicles when emergency slots are full
// Algorithm:
// 1. Inform user emergency slots are full
// 2. Offer choice: wait or park in nearest available slot
// 3. Add to waiting queue or find alternative slot
// Time Complexity: O(n) for finding alternative slot
// Space Complexity: O(1)
bool ParkingSystem::handleEmergencyFull(Vehicle& vehicle) {
    cout << "\n⚠ All emergency slots are occupied!\n";
    cout << "1. Wait for emergency slot to open\n";
    cout << "2. Park in nearest available slot (VIP or Normal)\n";
    cout << "Enter choice: ";
    
    int choice;
    cin >> choice;
    
    if (choice == 1) {
        // Add to emergency waiting queue
        emergencyWaitingQueue.enqueue(vehicle);
        cout << "✓ Added to emergency waiting queue.\n";
        cout << "  Queue position: " << emergencyWaitingQueue.getSize() << "\n";
        cout << "  You will be notified when an emergency slot opens.\n";
        return true;
    } else if (choice == 2) {
        // Find nearest available slot in VIP or Normal areas
        int nearestSlot = -1;
        int minDistance = TOTAL_SLOTS;
        
        // Search through all VIP and Normal slots (slots 3-19)
        for (int i = VIP_START; i <= NORMAL_END; i++) {
            if (!slots[i].occupied) {
                // Calculate distance from emergency area (slot 0)
                int distance = calculateDistance(i, EMERGENCY_START);
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestSlot = i;
                }
            }
        }
        
        if (nearestSlot != -1) {
            slots[nearestSlot].vehicle = vehicle;
            slots[nearestSlot].occupied = true;
            
            cout << "✓ Emergency vehicle parked at slot " << nearestSlot << "\n";
            cout << "  Distance from emergency area: " << minDistance << " slots\n";
            cout << "  Area: ";
            if (nearestSlot >= NORMAL_START) cout << "Normal";
            else if (nearestSlot >= VIP_START) cout << "VIP";
            cout << " (temporarily used for emergency)\n";
            return true;
        } else {
            cout << "✗ No available slots in VIP or Normal areas.\n";
            // Offer to add to emergency waiting queue as fallback
            cout << "Would you like to wait for emergency slot? (1=Yes, 0=No): ";
            int waitChoice;
            cin >> waitChoice;
            if (waitChoice == 1) {
                emergencyWaitingQueue.enqueue(vehicle);
                cout << "✓ Added to emergency waiting queue.\n";
                return true;
            }
            return false;
        }
    }
    
    return false;
}

// removeVehicle: Removes vehicle and handles queue management
// Algorithm:
// 1. Find vehicle by plate number using linear search
// 2. Remove from slot
// 3. Check emergency waiting queue first
// 4. Check regular queues based on slot area and priority
// Time Complexity: O(n) for searching vehicle
// Space Complexity: O(1)
void ParkingSystem::removeVehicle() {
    cout << "\n=== REMOVE VEHICLE ===\n";
    cout << "Enter license plate number: ";
    string plate;
    cin >> plate;
    
    bool found = false;
    
    // Linear search through all 20 slots
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        if (slots[i].occupied && slots[i].vehicle.plateNumber == plate) {
            found = true;
            
            // Store vehicle info before removing
            int removedPriority = slots[i].vehicle.priority;
            string removedPlate = slots[i].vehicle.plateNumber;
            
            // Remove vehicle
            slots[i].occupied = false;
            cout << "✓ Vehicle removed from slot " << i << "\n";
            cout << "  Plate: " << removedPlate << "\n";
            cout << "  Priority: ";
            if (removedPriority == 3) cout << "Emergency";
            else if (removedPriority == 2) cout << "VIP";
            else cout << "Normal";
            cout << "\n";
            
            // Check waiting queues in priority order
            Vehicle nextVehicle;
            bool parkedFromQueue = false;
            
            // 1. Check emergency waiting queue first (vehicles waiting for emergency slots)
            if (!emergencyWaitingQueue.isEmpty()) {
                nextVehicle = emergencyWaitingQueue.dequeue();
                slots[i].vehicle = nextVehicle;
                slots[i].occupied = true;
                parkedFromQueue = true;
                cout << "✓ Emergency vehicle from waiting queue parked at slot " << i << "\n";
                cout << "  Plate: " << nextVehicle.plateNumber << "\n";
            }
            // 2. Check if freed slot was in emergency area and regular emergency queue has vehicles
            else if (i <= EMERGENCY_END && !emergencyQueue.isEmpty()) {
                nextVehicle = emergencyQueue.dequeue();
                slots[i].vehicle = nextVehicle;
                slots[i].occupied = true;
                parkedFromQueue = true;
                cout << "✓ Emergency vehicle from queue parked at slot " << i << "\n";
                cout << "  Plate: " << nextVehicle.plateNumber << "\n";
            }
            // 3. Check VIP queue (if slot is in VIP area)
            else if (i >= VIP_START && i <= VIP_END && !vipQueue.isEmpty()) {
                nextVehicle = vipQueue.dequeue();
                slots[i].vehicle = nextVehicle;
                slots[i].occupied = true;
                parkedFromQueue = true;
                cout << "✓ VIP vehicle from queue parked at slot " << i << "\n";
                cout << "  Plate: " << nextVehicle.plateNumber << "\n";
            }
            // 4. Check normal queue (if slot is in normal area)
            else if (i >= NORMAL_START && i <= NORMAL_END && !normalQueue.isEmpty()) {
                nextVehicle = normalQueue.dequeue();
                slots[i].vehicle = nextVehicle;
                slots[i].occupied = true;
                parkedFromQueue = true;
                cout << "✓ Normal vehicle from queue parked at slot " << i << "\n";
                cout << "  Plate: " << nextVehicle.plateNumber << "\n";
            }
            
            if (!parkedFromQueue) {
                cout << "Slot " << i << " is now empty (no vehicles in queue).\n";
            }
            
            break;
        }
    }
    
    if (!found) {
        cout << "✗ Vehicle not found in parking system.\n";
        cout << "  Please check the license plate number.\n";
    }
}

// searchVehicle: Searches for vehicle by license plate
// Algorithm:
// 1. Linear search through all 20 slots
// 2. Compare plate numbers
// 3. Return slot information if found
// Time Complexity: O(n) where n = TOTAL_SLOTS
// Space Complexity: O(1)
void ParkingSystem::searchVehicle() {
    cout << "\n=== SEARCH VEHICLE ===\n";
    cout << "Enter license plate number: ";
    string plate;
    cin >> plate;
    
    bool found = false;
    
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        if (slots[i].occupied && slots[i].vehicle.plateNumber == plate) {
            found = true;
            cout << "\n✓ VEHICLE FOUND\n";
            cout << "  Slot Number: " << i << "\n";
            cout << "  License Plate: " << slots[i].vehicle.plateNumber << "\n";
            cout << "  Owner: " << slots[i].vehicle.ownerName << "\n";
            cout << "  Priority: ";
            switch(slots[i].vehicle.priority) {
                case 1: cout << "Normal"; break;
                case 2: cout << "VIP"; break;
                case 3: cout << "Emergency"; break;
            }
            cout << "\n";
            cout << "  Area: ";
            if (i <= EMERGENCY_END) cout << "Emergency";
            else if (i <= VIP_END) cout << "VIP";
            else cout << "Normal";
            cout << "\n";
            break;
        }
    }
    
    if (!found) {
        cout << "\n✗ Vehicle not found in parking slots.\n";
        cout << "Note: Vehicle might be in a waiting queue.\n";
    }
}

// findNearestFreeSlot: Finds nearest free slot from entrance using linear search
// Algorithm:
// 1. Use linear search with expanding radius
// 2. Start from entrance (slot 0)
// 3. Search slots at increasing distances
// 4. Return first free slot found
// Time Complexity: O(n²) worst case, O(1) best case
// Space Complexity: O(1)
void ParkingSystem::findNearestFreeSlot() {
    const int ENTRANCE_SLOT = 0; // Assuming entrance is at slot 0
    
    int nearestSlot = findNearestSlotLinear(ENTRANCE_SLOT);
    
    if (nearestSlot != -1) {
        int distance = calculateDistance(nearestSlot, ENTRANCE_SLOT);
        cout << "\n✓ NEAREST FREE SLOT FOUND\n";
        cout << "  Slot Number: " << nearestSlot << "\n";
        cout << "  Distance from entrance: " << distance << " slots\n";
        cout << "  Area: ";
        if (nearestSlot <= EMERGENCY_END) {
            cout << "Emergency";
            if (distance > 0) cout << " (close to entrance)";
        }
        else if (nearestSlot <= VIP_END) cout << "VIP";
        else cout << "Normal";
        cout << "\n";
        
        // Show which priority can use this slot
        cout << "  Available for: ";
        if (nearestSlot <= EMERGENCY_END) cout << "Emergency vehicles only";
        else if (nearestSlot <= VIP_END) cout << "VIP or Emergency vehicles";
        else cout << "Normal, VIP, or Emergency vehicles";
        cout << "\n";
    } else {
        cout << "\n✗ No free slots available.\n";
        cout << "  All " << TOTAL_SLOTS << " slots are occupied.\n";
    }
}

// displayParkingStatus: Shows current parking status for all 20 slots
// Algorithm:
// 1. Display header with statistics
// 2. Show each slot status with details
// 3. Show waiting queue sizes
// Time Complexity: O(n) where n = TOTAL_SLOTS
// Space Complexity: O(1)
void ParkingSystem::displayParkingStatus() {
    cout << "\n=== PARKING STATUS (20 SLOTS) ===\n";
    
    // Display statistics
    cout << "Total Slots: " << TOTAL_SLOTS << "\n";
    cout << "Emergency Slots: " << EMERGENCY_SLOTS << " (Slots " << EMERGENCY_START << "-" << EMERGENCY_END << ")\n";
    cout << "VIP Slots: " << VIP_SLOTS << " (Slots " << VIP_START << "-" << VIP_END << ")\n";
    cout << "Normal Slots: " << NORMAL_SLOTS << " (Slots " << NORMAL_START << "-" << NORMAL_END << ")\n";
    
    int freeSlots = getFreeSlotsCount();
    int occupiedSlots = getOccupiedSlotsCount();
    cout << "\nOccupied Slots: " << occupiedSlots << " | Free Slots: " << freeSlots << "\n";
    
    // Display occupancy percentage
    if (TOTAL_SLOTS > 0) {
        int occupancyRate = (occupiedSlots * 100) / TOTAL_SLOTS;
        cout << "Occupancy Rate: " << occupancyRate << "%\n";
    }
    
    // Display emergency slots (0-2)
    cout << "\n=== EMERGENCY SLOTS (0-2) ===\n";
    int emergencyFree = 0;
    for (int i = EMERGENCY_START; i <= EMERGENCY_END; i++) {
        cout << "Slot " << i << ": ";
        if (slots[i].occupied) {
            cout << "[" << slots[i].vehicle.plateNumber << "] ";
            cout << slots[i].vehicle.ownerName;
            // Show if non-emergency vehicle is parked here
            if (slots[i].vehicle.priority != 3) {
                cout << " (Non-emergency vehicle)";
            }
        } else {
            cout << "EMPTY";
            emergencyFree++;
        }
        cout << "\n";
    }
    cout << "Free emergency slots: " << emergencyFree << "/" << EMERGENCY_SLOTS << "\n";
    
    // Display VIP slots (3-9)
    cout << "\n=== VIP SLOTS (3-9) ===\n";
    int vipFree = 0;
    for (int i = VIP_START; i <= VIP_END; i++) {
        cout << "Slot " << i << ": ";
        if (slots[i].occupied) {
            cout << "[" << slots[i].vehicle.plateNumber << "] ";
            cout << slots[i].vehicle.ownerName;
            // Show if emergency vehicle is parked here
            if (slots[i].vehicle.priority == 3) {
                cout << " (Emergency vehicle)";
            } else if (slots[i].vehicle.priority == 1) {
                cout << " (Normal vehicle)";
            }
        } else {
            cout << "EMPTY";
            vipFree++;
        }
        cout << "\n";
    }
    cout << "Free VIP slots: " << vipFree << "/" << VIP_SLOTS << "\n";
    
    // Display normal slots (10-19)
    cout << "\n=== NORMAL SLOTS (10-19) ===\n";
    int normalFree = 0;
    for (int i = NORMAL_START; i <= NORMAL_END; i++) {
        cout << "Slot " << i << ": ";
        if (slots[i].occupied) {
            cout << "[" << slots[i].vehicle.plateNumber << "] ";
            cout << slots[i].vehicle.ownerName;
            // Show if emergency or VIP vehicle is parked here
            if (slots[i].vehicle.priority == 3) {
                cout << " (Emergency vehicle)";
            } else if (slots[i].vehicle.priority == 2) {
                cout << " (VIP vehicle)";
            }
        } else {
            cout << "EMPTY";
            normalFree++;
        }
        cout << "\n";
    }
    cout << "Free normal slots: " << normalFree << "/" << NORMAL_SLOTS << "\n";
    
    // Display waiting queues
    displayWaitingQueues();
}

// displayWaitingQueues: Shows vehicles in waiting queues
// Algorithm:
// 1. Display each queue with vehicle count
// 2. Show details of vehicles in each queue
// Time Complexity: O(m) where m = total vehicles in queues
// Space Complexity: O(1)
void ParkingSystem::displayWaitingQueues() {
    cout << "\n=== WAITING QUEUES ===\n";
    cout << "Emergency Waiting Queue: " << emergencyWaitingQueue.getSize() << " vehicles\n";
    cout << "  (Emergency vehicles waiting specifically for emergency slots)\n";
    
    cout << "\nRegular Queues (sorted by priority):\n";
    cout << "Emergency Queue: " << emergencyQueue.getSize() << " vehicles\n";
    cout << "VIP Queue: " << vipQueue.getSize() << " vehicles\n";
    cout << "Normal Queue: " << normalQueue.getSize() << " vehicles\n";
    cout << "Total vehicles waiting: " << 
        emergencyWaitingQueue.getSize() + emergencyQueue.getSize() + 
        vipQueue.getSize() + normalQueue.getSize() << "\n";
}

// getFreeSlotsCount: Counts total free slots
// Algorithm:
// 1. Iterate through all 20 slots
// 2. Count unoccupied slots
// Time Complexity: O(n) where n = TOTAL_SLOTS
// Space Complexity: O(1)
int ParkingSystem::getFreeSlotsCount() const {
    int count = 0;
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        if (!slots[i].occupied) {
            count++;
        }
    }
    return count;
}

// getOccupiedSlotsCount: Counts total occupied slots
// Algorithm:
// 1. Iterate through all 20 slots
// 2. Count occupied slots
// Time Complexity: O(n) where n = TOTAL_SLOTS
// Space Complexity: O(1)
int ParkingSystem::getOccupiedSlotsCount() const {
    int count = 0;
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        if (slots[i].occupied) {
            count++;
        }
    }
    return count;
}

// isParkingFull: Checks if parking is completely full
// Algorithm:
// 1. Check if all 20 slots are occupied
// Time Complexity: O(1) - uses counter
// Space Complexity: O(1)
bool ParkingSystem::isParkingFull() const {
    return getOccupiedSlotsCount() == TOTAL_SLOTS;
}

// getFreeSlotsForPriority: Counts free slots for specific priority
// Algorithm:
// 1. Determine priority area range
// 2. Count unoccupied slots in range
// Time Complexity: O(m) where m = slots in priority range
// Space Complexity: O(1)
int ParkingSystem::getFreeSlotsForPriority(int priority) const {
    int startSlot, endSlot;
    
    switch (priority) {
        case 3: // Emergency
            startSlot = EMERGENCY_START;
            endSlot = EMERGENCY_END;
            break;
        case 2: // VIP
            startSlot = VIP_START;
            endSlot = VIP_END;
            break;
        case 1: // Normal
            startSlot = NORMAL_START;
            endSlot = NORMAL_END;
            break;
        default:
            return 0;
    }
    
    int count = 0;
    for (int i = startSlot; i <= endSlot; i++) {
        if (!slots[i].occupied) {
            count++;
        }
    }
    return count;
}

// displayEmergencySlots: Displays only emergency slots
// Algorithm:
// 1. Iterate through emergency slot range (0-2)
// 2. Display status of each slot
// Time Complexity: O(m) where m = EMERGENCY_SLOTS
// Space Complexity: O(1)
void ParkingSystem::displayEmergencySlots() const {
    cout << "\n=== EMERGENCY SLOTS (0-2) ===\n";
    for (int i = EMERGENCY_START; i <= EMERGENCY_END; i++) {
        cout << "Slot " << i << ": ";
        if (slots[i].occupied) {
            cout << "[" << slots[i].vehicle.plateNumber << "] ";
            cout << slots[i].vehicle.ownerName;
            cout << " (Priority: ";
            if (slots[i].vehicle.priority == 3) cout << "Emergency";
            else if (slots[i].vehicle.priority == 2) cout << "VIP";
            else cout << "Normal";
            cout << ")";
        } else {
            cout << "EMPTY";
        }
        cout << "\n";
    }
}

// displayVipSlots: Displays only VIP slots
// Algorithm:
// 1. Iterate through VIP slot range (3-9)
// 2. Display status of each slot
// Time Complexity: O(m) where m = VIP_SLOTS
// Space Complexity: O(1)
void ParkingSystem::displayVipSlots() const {
    cout << "\n=== VIP SLOTS (3-9) ===\n";
    for (int i = VIP_START; i <= VIP_END; i++) {
        cout << "Slot " << i << ": ";
        if (slots[i].occupied) {
            cout << "[" << slots[i].vehicle.plateNumber << "] ";
            cout << slots[i].vehicle.ownerName;
            cout << " (Priority: ";
            if (slots[i].vehicle.priority == 3) cout << "Emergency";
            else if (slots[i].vehicle.priority == 2) cout << "VIP";
            else cout << "Normal";
            cout << ")";
        } else {
            cout << "EMPTY";
        }
        cout << "\n";
    }
}

// displayNormalSlots: Displays only normal slots
// Algorithm:
// 1. Iterate through normal slot range (10-19)
// 2. Display status of each slot
// Time Complexity: O(m) where m = NORMAL_SLOTS
// Space Complexity: O(1)
void ParkingSystem::displayNormalSlots() const {
    cout << "\n=== NORMAL SLOTS (10-19) ===\n";
    for (int i = NORMAL_START; i <= NORMAL_END; i++) {
        cout << "Slot " << i << ": ";
        if (slots[i].occupied) {
            cout << "[" << slots[i].vehicle.plateNumber << "] ";
            cout << slots[i].vehicle.ownerName;
            cout << " (Priority: ";
            if (slots[i].vehicle.priority == 3) cout << "Emergency";
            else if (slots[i].vehicle.priority == 2) cout << "VIP";
            else cout << "Normal";
            cout << ")";
        } else {
            cout << "EMPTY";
        }
        cout << "\n";
    }
}