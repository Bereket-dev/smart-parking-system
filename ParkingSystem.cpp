#include "ParkingSystem.h"
#include <iostream>
// #include <queue>
#include <climits>
#include <cmath>

using namespace std;

// Initialize parking system
ParkingSystem::ParkingSystem(int totalSlots)
{
    head = tail = nullptr;
    graph.resize(totalSlots + 1); // adjacency list
    distanceFromGates.resize(totalSlots + 1);

    recentTop = nullptr;
    historyHead = nullptr;
    historyTail = nullptr;
}

// Build grid graph (rows x cols) with uniform distances
void ParkingSystem::buildGridGraph(int rows, int cols, int dRow, int dCol)
{
    int id = 1;
    vector<pair<int, int>> coordinates(rows * cols + 1);

    // Create slots as linked list
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            ParkingSlot *slot = new ParkingSlot(id);
            coordinates[id] = {r, c};

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

    // Build adjacency list
    for (int i = 1; i <= rows * cols; i++)
    {
        int r = coordinates[i].first;
        int c = coordinates[i].second;

        for (int j = 1; j <= rows * cols; j++)
        {
            if (i == j)
                continue;
            int rr = coordinates[j].first;
            int cc = coordinates[j].second;

            int dr = abs(r - rr);
            int dc = abs(c - cc);
            int dist = dr * dRow + dc * dCol; // use row/col spacing

            // Neighbor if adjacent (up, down, left, right)
            if ((abs(r - rr) + abs(c - cc)) == 1)
                graph[i].push_back({j, dist});
        }
    }
}

// Add gate
void ParkingSystem::addGate(int slotId)
{
    gateSlots.push_back(slotId);
}

// Compute shortest distance from each gate using BFS
void ParkingSystem::computeDistances()
{
    int totalSlots = graph.size() - 1;

    for (size_t g = 0; g < gateSlots.size(); g++)
    {
        int gateId = gateSlots[g];
        vector<int> dist(totalSlots + 1, INT_MAX);
        queue<int> q;

        dist[gateId] = 0;
        q.push(gateId);

        while (!q.empty())
        {
            int curr = q.front();
            q.pop();
            for (auto &neighbor : graph[curr])
            {
                int neighborId = neighbor.first;
                if (dist[neighborId] == INT_MAX)
                {
                    dist[neighborId] = dist[curr] + neighbor.second;
                    q.push(neighborId);
                }
            }
        }

        // Store distances from this gate
        for (int i = 1; i <= totalSlots; i++)
        {
            distanceFromGates[i].resize(gateSlots.size());
            distanceFromGates[i][g] = dist[i];
        }
    }
}

// Add vehicle
void ParkingSystem::addVehicle(Vehicle v)
{
    if (v.priority == 3)
        normalQueue.enqueue(v);
    else
        priorityQueue.push(v);
}

// Find nearest free slot from a gate using BFS distances
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

// Assign vehicle from a specific gate
void ParkingSystem::assignSlot(int gateIndex)
{
    Vehicle v;
    if (!priorityQueue.empty())
    {
        v = priorityQueue.top();
        priorityQueue.pop();
    }
    else if (!normalQueue.isEmpty())
    {
        v = normalQueue.dequeue();
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
        return;
    }

    ParkingSlot *curr = head;
    while (curr)
    {
        if (curr->slotId == slotId)
        {
            curr->isFree = false;
            break;
        }
        curr = curr->next;
    }

    string action = "PARK: " + v.plateNumber + " at slot " + to_string(slotId);
    recordAction(action);

    cout << "Vehicle " << v.plateNumber
         << " parked at slot " << slotId
         << " (distance " << distanceFromGates[slotId][gateIndex]
         << " from gate " << gateIndex + 1 << ")\n";
}

// Remove vehicle
void ParkingSystem::removeVehicle(string plate)
{
    for (ParkingSlot *curr = head; curr; curr = curr->next)
    {
        if (!curr->isFree)
        { // you may want to check vehicleMap if implemented
            curr->isFree = true;

            string action = "EXIT: " + plate + " from slot " + to_string(curr->slotId);
            recordAction(action);
        }
    }
    cout << "Vehicle removed.\n";
}

// Display slots and distances from all gates
void ParkingSystem::displaySlots()
{
    ParkingSlot *curr = head;
    while (curr)
    {
        cout << "Slot " << curr->slotId
             << " | Status: " << (curr->isFree ? "Free" : "Occupied")
             << " | Distances: ";
        for (size_t g = 0; g < gateSlots.size(); g++)
            cout << distanceFromGates[curr->slotId][g] << (g + 1 == gateSlots.size() ? "" : ", ");
        cout << endl;
        curr = curr->next;
    }
}

void ParkingSystem::recordAction(string action)
{

    HistoryNode *newNode = new HistoryNode{action, recentTop};
    recentTop = newNode;

    HistoryNode *newHist = new HistoryNode{action, nullptr};
    if (historyHead == nullptr)
    {
        historyHead = historyTail = newHist;
    }
    else
    {
        historyTail->next = newHist;
        historyTail = newHist;
    }
}

void ParkingSystem::pushAction(string action)
{
    recordAction(action);
}

string ParkingSystem::popAction()
{
    if (recentTop == nullptr)
        return "No actions";
    HistoryNode *temp = recentTop;
    string action = temp->action;
    recentTop = recentTop->next;
    delete temp;
    return action;
}

string ParkingSystem::peekAction()
{
    if (recentTop == nullptr)
        return "No actions";
    return recentTop->action;
}

void ParkingSystem::appendHistory(string action)
{
    recordAction(action);
}

// Show Recent Action
void ParkingSystem::showRecentAction()
{
    cout << "Recent Action: " << peekAction() << endl;
}

// Show Full History
void ParkingSystem::showFullHistory()
{
    cout << "\nFull Parking History:\n";
    if (historyHead == nullptr)
    {
        cout << "No history available.\n";
        return;
    }
    HistoryNode *temp = historyHead;
    while (temp != nullptr)
    {
        cout << temp->action << endl;
        temp = temp->next;
    }
}
