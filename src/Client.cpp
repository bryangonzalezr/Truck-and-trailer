#include "Client.h"
#include <cmath>

// Default constructor
Client::Client() {}

// Parameterized constructor
Client::Client(int number, double x, double y, double demand, bool truck_customer)
    : number(number), x(x), y(y), demand(demand), truck_customer(truck_customer) {}

// Method to calculate distance to another client
double Client::distance_to(Client* other) {
    return std::sqrt(std::pow(x - other->x, 2) + std::pow(y - other->y, 2));
}