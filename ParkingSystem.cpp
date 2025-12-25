#include "ParkingSystem.h"
#include <iostream>
#include <climits>
#include <cmath>

using namespace std;

ParkingSystem::ParkingSystem(int totalSlots)
{
    head = tail = nullptr;
    totalSlotsCount = totalSlots;
    gateCount = 0;
    gateHead = nullptr;

    graph = new AdjNode *[totalSlots + 1];
    for (int i = 0; i <= totalSlots; i++)
        graph[i] = nullptr;

    distanceFromGates = new int *[totalSlots + 1];
    for (int i = 0; i <= totalSlots; i++)
        distanceFromGates[i] = nullptr;

    recentTop = nullptr;
    historyHead = historyTail = nullptr;
}

ParkingSystem::~ParkingSystem()
{
    for (int i = 0; i <= totalSlotsCount; i++)
    {
        AdjNode *curr = graph[i];
        while (curr)
        {
            AdjNode *temp = curr;
            curr = curr->next;
            delete temp;
        }
    }
    delete[] graph;

    for (int i = 0; i <= totalSlotsCount; i++)
    {
        if (distanceFromGates[i])
            delete[] distanceFromGates[i];
    }
    delete[] distanceFromGates;

    GateNode *gCurr = gateHead;
    while (gCurr)
    {
        GateNode *temp = gCurr;
        gCurr = gCurr->next;
        delete temp;
    }

    while (recentTop)
        popAction();
    HistoryNode *hCurr = historyHead;
    while (hCurr)
    {
        HistoryNode *temp = hCurr;
        hCurr = hCurr->next;
        delete temp;
    }
}

void ParkingSystem::buildGridGraph(int rows, int cols, int dRow, int dCol)
{
    int id = 1;
    struct Coord
    {
        int r, c;
    };
    Coord *coords = new Coord[rows * cols + 1];

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            ParkingSlot *slot = new ParkingSlot(id);
            coords[id] = {r, c};

            if (!head)
                head = tail = slot;
            else
            {
                tail->next = slot;
                slot->prev = tail;
                tail = slot;
            }
            id++;
        }
    }

    for (int i = 1; i <= rows * cols; i++)
    {
        for (int j = 1; j <= rows * cols; j++)
        {
            if (i == j)
                continue;
            int dist = abs(coords[i].r - coords[j].r) * dRow + abs(coords[i].c - coords[j].c) * dCol;

            if ((abs(coords[i].r - coords[j].r) + abs(coords[i].c - coords[j].c)) == 1)
            {
                AdjNode *newNode = new AdjNode{j, dist, graph[i]};
                graph[i] = newNode;
            }
        }
    }
    delete[] coords;
}

void ParkingSystem::addGate(int slotId)
{
    GateNode *newNode = new GateNode{slotId, nullptr};
    if (!gateHead)
        gateHead = newNode;
    else
    {
        GateNode *temp = gateHead;
        while (temp->next)
            temp = temp->next;
        temp->next = newNode;
    }
    gateCount++;
}

void ParkingSystem::computeDistances()
{
    GateNode *currGate = gateHead;
    for (int g = 0; g < gateCount; g++)
    {
        int gateId = currGate->slotId;

        int *dist = new int[totalSlotsCount + 1];
        for (int i = 0; i <= totalSlotsCount; i++)
            dist[i] = INT_MAX;

        queue<int> q;
        dist[gateId] = 0;
        q.push(gateId);

        while (!q.empty())
        {
            int curr = q.front();
            q.pop();

            AdjNode *neighbor = graph[curr];
            while (neighbor)
            {
                if (dist[neighbor->neighborId] == INT_MAX)
                {
                    dist[neighbor->neighborId] = dist[curr] + neighbor->weight;
                    q.push(neighbor->neighborId);
                }
                neighbor = neighbor->next;
            }
        }

        for (int i = 1; i <= totalSlotsCount; i++)
        {
            if (!distanceFromGates[i])
                distanceFromGates[i] = new int[gateCount];
            distanceFromGates[i][g] = dist[i];
        }
        delete[] dist;
        currGate = currGate->next;
    }
}

void ParkingSystem::addVehicle(Vehicle v)
{
    priorityQueue.enqueue(v);
}

int ParkingSystem::findNearestAvailableSlot(int gateIndex)
{
    ParkingSlot *curr = head;
    int bestSlot = -1;
    int minDist = INT_MAX;

    while (curr)
    {
        int id = curr->slotId;
        if (curr->isFree && distanceFromGates[id][gateIndex] < minDist)
        {
            minDist = distanceFromGates[id][gateIndex];
            bestSlot = id;
        }
        curr = curr->next;
    }
    return bestSlot;
}

void ParkingSystem::assignSlot(int gateIndex)
{
    Vehicle v;
    if (!priorityQueue.isEmpty())
    {
        v = priorityQueue.top();
    }
    else
    {
        cout << "No vehicles waiting.\n";
        return;
    }

    int slotId = findNearestAvailableSlot(gateIndex);
    if (slotId == -1)
    {
        cout << "Parking full.\n";
        waitingQueue.enqueue(v);
        return;
    }
    priorityQueue.dequeue(); // remove from wait if there is a free space only

    ParkingSlot *curr = head;
    while (curr)
    {
        if (curr->slotId == slotId)
        {
            curr->isFree = false;
            curr->parkedPlate = v.plateNumber;
            break;
        }
        curr = curr->next;
    }

    recordAction("PARK: " + v.plateNumber + " at slot " + to_string(slotId));
    cout << "Vehicle " << v.plateNumber << " parked at slot " << slotId << "\n";
}

void ParkingSystem::removeVehicle(string plate)
{
    bool found = false;
    for (ParkingSlot *curr = head; curr; curr = curr->next)
    {
        if (!curr->isFree && curr->parkedPlate == plate)
        {
            if (!waitingQueue.isEmpty())
            {
                Vehicle v = waitingQueue.top();
                waitingQueue.dequeue();
                curr->parkedPlate = v.plateNumber;
                recordAction("EXIT: " + plate + " from slot " + to_string(curr->slotId));
                recordAction("PARK: " + v.plateNumber + " at slot " + to_string(curr->slotId));
            }
            else
            {
                curr->isFree = true;
                curr->parkedPlate = "";
                recordAction("EXIT: " + plate + " from slot " + to_string(curr->slotId));
            }

            cout << "Success: Vehicle removed from slot " << curr->slotId << ".\n";
            found = true;

            break;
        }
    }
    if (!found)
        cout << "Error: Vehicle " << plate << " not found.\n";
}

void ParkingSystem::displaySlots()
{
    ParkingSlot *curr = head;
    while (curr)
    {
        cout << "Slot " << curr->slotId << " | " << (curr->isFree ? "Free" : "Occupied");
        cout << " | Distances: ";
        for (int g = 0; g < gateCount; g++)
        {
            cout << distanceFromGates[curr->slotId][g] << (g + 1 == gateCount ? "" : ", ");
        }
        cout << endl;
        curr = curr->next;
    }
}

void ParkingSystem::recordAction(string action)
{
    HistoryNode *newNode = new HistoryNode{action, recentTop};
    recentTop = newNode;

    HistoryNode *newHist = new HistoryNode{action, nullptr};
    if (!historyHead)
        historyHead = historyTail = newHist;
    else
    {
        historyTail->next = newHist;
        historyTail = newHist;
    }
}

string ParkingSystem::popAction()
{
    if (!recentTop)
        return "No actions";
    HistoryNode *temp = recentTop;
    string action = temp->action;
    recentTop = recentTop->next;
    delete temp;
    return action;
}

string ParkingSystem::peekAction()
{
    return recentTop ? recentTop->action : "No actions";
}

void ParkingSystem::pushAction(string action) { recordAction(action); }
void ParkingSystem::appendHistory(string action) { recordAction(action); }
void ParkingSystem::showRecentAction() { cout << "Recent: " << peekAction() << endl; }

void ParkingSystem::showFullHistory()
{
    cout << "\nFull Parking History:\n";
    HistoryNode *temp = historyHead;
    if (!temp)
        cout << "No history available.\n";
    while (temp)
    {
        cout << temp->action << endl;
        temp = temp->next;
    }
}