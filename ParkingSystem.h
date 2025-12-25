#ifndef PARKINGSYSTEM_H
#define PARKINGSYSTEM_H

#include <iostream>
#include <string>
#include <queue>
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

struct GateNode
{
    int slotId;
    GateNode *next;
};

struct AdjNode
{
    int neighborId;
    int weight;
    AdjNode *next;
};

class ParkingSystem
{
private:
    ParkingSlot *head;
    ParkingSlot *tail;

    AdjNode **graph;
    int totalSlotsCount;

    int **distanceFromGates;
    int gateCount;

    Queue priorityQueue;
    Queue waitingQueue;

    struct HistoryNode
    {
        string action;
        HistoryNode *next;
    };

    HistoryNode *recentTop;
    HistoryNode *historyHead;
    HistoryNode *historyTail;

public:
    GateNode *gateHead;

    ParkingSystem(int totalSlots);
    ~ParkingSystem();

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