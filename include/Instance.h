// Instance.h
#ifndef INSTANCE_H
#define INSTANCE_H

#include <vector>
#include "Client.h"

class Instance {
public:
    int N_trucks;
    double truck_capacity;
    int N_trailers;
    double trailer_capacity;
    int N_clients;
    Client deposit;
    std::vector<Client*> clients;

    Instance(int N_trucks, double truck_capacity, int N_trailers, double trailer_capacity, std::vector<Client*> clients);

    int getTotalDemand(); // Add this line

    ~Instance();
};

#endif // INSTANCE_H
