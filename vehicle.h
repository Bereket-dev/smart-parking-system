#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
using namespace std;

class Vehicle
{
public:
    string plateNumber;
    int priority; // 1 = VIP, 2 = Electric, 3 = Normal

    Vehicle() {}
    Vehicle(string plate, int p)
    {
        plateNumber = plate;
        priority = p;
    }
};

#endif