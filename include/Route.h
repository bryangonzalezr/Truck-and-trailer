#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include "Truck.h"
#include "Trailer.h"
#include "Instance.h"

class Route {
public:
    Truck* truck;
    Trailer* trailer;
    std::vector<Client*> clients;
    bool trailer_attached;
    int current_truck_capacity;
    int current_trailer_capacity;
    int current_capacity;
    Client* trailer_location;

    Route(Truck* truck, Trailer* trailer = nullptr);
    void add_client(Client* client, Instance& instance);
    double total_distance(Client& deposit);
    int total_demand();
    int get_current_capacity();
};

#endif // ROUTE_H
