#include <iostream>
#include <string>
#include "ParkingSystem.h"

using namespace std;

int main()
{
    int rows, cols, dRow, dCol;
    cout << "=== Smart Parking System Configuration ===\n";
    cout << "Enter number of rows: ";
    cin >> rows;
    cout << "Enter distance between rows: ";
    cin >> dRow;
    cout << "Enter number of columns: ";
    cin >> cols;
    cout << "Enter distance between columns: ";
    cin >> dCol;

    int totalSlots = rows * cols;
    ParkingSystem system(totalSlots);
    system.buildGridGraph(rows, cols, dRow, dCol);

    int gates;
    cout << "Enter number of gates: ";
    cin >> gates;

    for (int i = 0; i < gates; i++)
    {
        string side;
        int pos;
        int slotId = -1;
        cout << "Gate " << i + 1 << " - side (left/right/front/back): ";
        cin >> side;

        if (side == "left" || side == "right")
        {
            cout << "Enter row index (0 to " << rows - 1 << "): ";
            cin >> pos;
            slotId = (side == "left") ? pos * cols + 1 : pos * cols + cols;
        }
        else if (side == "front" || side == "back")
        {
            cout << "Enter column index (0 to " << cols - 1 << "): ";
            cin >> pos;
            slotId = (side == "front") ? pos + 1 : (rows - 1) * cols + pos + 1;
        }
        else
        {
            cout << "Invalid side! Skipping this gate.\n";
            continue;
        }

        system.addGate(slotId);
    }

    system.computeDistances();

    int choice;
    do
    {
        cout << "\n=== Parking Menu ===\n";
        cout << "1. Add Vehicle\n";
        cout << "2. Assign Slot To Vehicle\n";
        cout << "3. Remove Vehicle (Exit)\n";
        cout << "4. Display All Slots Status\n";
        cout << "5. Show Most Recent Action\n";
        cout << "6. Show Full History\n";
        cout << "0. Exit Program\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1)
        {
            string plate;
            int pr;
            cout << "Enter vehicle plate: ";
            cin >> plate;
            cout << "Enter priority (1: Emergency, 2: VIP, 3: Normal): ";
            cin >> pr;
            system.addVehicle(Vehicle(plate, pr));
        }
        else if (choice == 2)
        {
            int gateNum;
            cout << "Enter gate number vehicle entered from (1 to " << gates << "): ";
            cin >> gateNum;

            if (gateNum < 1 || gateNum > gates)
            {
                cout << "Invalid gate number.\n";
            }
            else
            {
                system.assignSlot(gateNum - 1);
            }
        }
        else if (choice == 3)
        {
            string plate;
            cout << "Enter vehicle plate to remove: ";
            cin >> plate;
            system.removeVehicle(plate);
        }
        else if (choice == 4)
        {
            system.displaySlots();
        }
        else if (choice == 5)
        {
            system.showRecentAction();
        }
        else if (choice == 6)
        {
            system.showFullHistory();
        }
    } while (choice != 0);

    return 0;
}