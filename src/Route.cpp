#include "Route.h"

using namespace std;

Route::Route(Truck* truck, Trailer* trailer) 
    : truck(truck), trailer(trailer), trailer_attached(trailer != nullptr), 
      current_truck_capacity(truck->capacity), 
      current_trailer_capacity(trailer != nullptr ? trailer->capacity : 0),
      current_capacity(current_truck_capacity + current_trailer_capacity), 
      trailer_location(nullptr) {
    Client deposit;
    current_capacity -= deposit.demand;
}

void Route::add_client(Client* client, Instance& instance) {
    if(!trailer_attached && (trailer_location != nullptr) && (trailer_location->number == client->number)){
        trailer_attached = true;
        trailer_location = nullptr;
        if(current_truck_capacity <= current_trailer_capacity - instance.trailer_capacity){
            current_truck_capacity += current_trailer_capacity - instance.trailer_capacity;
            current_trailer_capacity = instance.trailer_capacity;
        }else{
            current_trailer_capacity = current_truck_capacity;
            current_truck_capacity = instance.truck_capacity;
        }
    } else {
        if (client->truck_customer && trailer_attached) {
            trailer_attached = false;
            trailer_location = clients.empty() ? nullptr : clients.back();
        }
        if(!trailer_attached) {
            current_truck_capacity -= client->demand;
        } else {
            if(current_trailer_capacity >= client->demand) {
                current_trailer_capacity -= client->demand;
            } else {
                current_truck_capacity -= (client->demand - current_trailer_capacity);
                current_trailer_capacity = 0;
            }
        }
        current_capacity -= client->demand;
    }
    clients.push_back(client);
}

double Route::total_distance(Client& deposit) {
    double total = clients[0]->distance_to(&deposit);
    for (size_t i = 0; i < clients.size() - 1; ++i) {
        total += clients[i]->distance_to(clients[i + 1]);
    }
    return total;
}

int Route::total_demand() {
    int total = 0;
    for (Client* client : clients) {
        total += client->demand;
    }
    return total;
}

int Route::get_current_capacity() {
    return trailer_attached ? current_trailer_capacity + current_truck_capacity : current_truck_capacity;
}
