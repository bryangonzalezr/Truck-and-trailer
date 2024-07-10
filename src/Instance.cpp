// Instance.cpp
#include "Instance.h"

Instance::Instance(int N_trucks, double truck_capacity, int N_trailers, double trailer_capacity, std::vector<Client*> clients)
    : N_trucks(N_trucks), truck_capacity(truck_capacity), N_trailers(N_trailers), trailer_capacity(trailer_capacity), clients(clients) {
}

Instance::~Instance() {
    for (Client* client : clients) {
        delete client;
    }
}

int Instance::getTotalDemand() {
    int totalDemand = 0;
    for (Client* client : clients) {
        totalDemand += client->demand;
    }
    return totalDemand;
}

