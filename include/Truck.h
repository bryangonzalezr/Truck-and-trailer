#ifndef TRUCK_H
#define TRUCK_H

#include <vector>
#include "Client.h"

class Truck {
public:
    double capacity;
    std::vector<Client*> route;

    Truck(double capacity);
    int total_demand();
};

#endif // TRUCK_H