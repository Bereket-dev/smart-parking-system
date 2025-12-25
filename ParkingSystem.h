#ifndef PARKINGSYSTEM_H
#define PARKINGSYSTEM_H

#include <unordered_map>
#include <queue>
#include <vector>
#include <limits>
#include "Vehicle.h"
#include "Queue.h"
#include "ParkingSlot.h"

using namespace std;

struct ComparePriority {
    bool operator()(Vehicle a, Vehicle b) {
        return a.priority > b.priority;
    }
};

class ParkingSystem {
private:
    ParkingSlot* head;
    ParkingSlot* tail;

    vector<vector<pair<int,int>>> graph;       // adjacency list
    vector<vector<int>> distanceFromGates;     // distanceFromGates[slotId][gateIndex]

    Queue normalQueue;
    priority_queue<Vehicle, vector<Vehicle>, ComparePriority> priorityQueue;
    unordered_map<string, int> vehicleMap;

public:
    vector<int> gateSlots;                     // stores slot IDs of gates

    ParkingSystem(int totalSlots);

    void buildGridGraph(int rows, int cols, int dRow, int dCol);
    void addGate(int slotId);                       // add gate
    void computeDistances();                        // compute distance from all gates

    void addVehicle(Vehicle v);
    void assignSlot(int gateIndex);                 // assign vehicle from a specific gate
    int findNearestAvailableSlot(int gateIndex);   // nearest slot from given gate
    void removeVehicle(string plate);
    void displaySlots();
};

#endif
