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

struct ComparePriority
{
    bool operator()(Vehicle a, Vehicle b)
    {
        return a.priority > b.priority;
    }
};

class ParkingSystem
{
private:
    ParkingSlot *head;
    ParkingSlot *tail;

    vector<vector<pair<int, int>>> graph; 
    vector<vector<int>> distanceFromGates; 

    Queue normalQueue;
    priority_queue<Vehicle, vector<Vehicle>, ComparePriority> priorityQueue;
    unordered_map<string, int> vehicleMap;

    struct HistoryNode
    {
        string action;
        HistoryNode *next;
    };

    HistoryNode *recentTop;
    HistoryNode *historyHead;
    HistoryNode *historyTail;

public:
    vector<int> gateSlots; 

    ParkingSystem(int totalSlots);

    void buildGridGraph(int rows, int cols, int dRow, int dCol);
    void addGate(int slotId); 
    void computeDistances(); 

    void addVehicle(Vehicle v);
    void assignSlot(int gateIndex);              
    int findNearestAvailableSlot(int gateIndex); 
    void removeVehicle(string plate);
    void displaySlots();

    string popAction();
    string peekAction();
    void recordAction(string action);
    void pushAction(string action);
    void appendHistory(string action);
    void showRecentAction();
    void showFullHistory();
};

#endif
