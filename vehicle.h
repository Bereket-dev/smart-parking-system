#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
using namespace std;

struct Vehicle {
    string plateNumber;
    string ownerName;
    int priority; // 1 = Normal, 2 = VIP, 3 = Emergency
};

#endif