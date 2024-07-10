#include "Truck.h"

Truck::Truck(double capacity) : capacity(capacity) {}

int Truck::total_demand() {
    int total = 0;
    for (Client* client : route) {
        total += client->demand;
    }
    return total;
}
