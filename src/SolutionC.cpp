#include "Solution.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem> // C++17 header

using namespace std;


void Solution::construct_routes(std::vector<Client*>& clients, Truck* truck, Trailer* trailer) {
    return;
}

void Solution::precompute_distances(Instance& instance) {
    for (int i = 0; i <= instance.N_clients; ++i) {
        Client* client1 = (i == 0) ? &instance.deposit : instance.clients[i - 1];
        distances[i][0] = client1->distance_to(&instance.deposit);
        for (int j = i + 1; j <= instance.N_clients; ++j) {
            Client* client2 = instance.clients[j - 1];
            double distance = client1->distance_to(client2);
            distances[i][j] = distance;
            distances[j][i] = distance;
        }
    }
}

double Solution::evaluate(Instance& instance) {
    isFeasible = true;
    double total_distance = 0;
    for (Route& route : routes) {
        total_distance += route.total_distance(instance.deposit);
    }
    //check if all instance.clients are on route.assigned_clients
    for (Client* client : instance.clients) {
        if (assigned_clients.count(client) == 0) {
            total_distance += 1000;
            isFeasible = false;
            
        }
    }

    return total_distance;
    
}

Client* Solution::select_next_client(Instance& instance, Route& route) {
    int current_location = route.clients.empty() ? 0 : route.clients.back()->number;
    vector<pair<double, Client*>> sorted_clients;

        for (Client* client : instance.clients) {
        if (assigned_clients.count(client) == 0 
        // && ((client->truck_customer && route.clients.empty() && !route.trailer_attached && !route.trailer_location) ? false : true) 
        && (client->truck_customer? route.current_truck_capacity : route.get_current_capacity()) >= client->demand){
            
            // cout << "Checking client: " << client->number <<" demand: "<<client->demand <<" truck client:"<<client->truck_customer<<endl;
            double distance = distances[current_location][client->number];
            double distance_back = (route.trailer_location ==nullptr) ? 
                distances[client->number][0] : 
                distances[client->number][route.trailer_location->number];
            double fitness = (distance)* (distance_back/route.get_current_capacity());

            sorted_clients.emplace_back(fitness, client);
        }

    }
    sort(sorted_clients.begin(), sorted_clients.end());
    

    double average = instance.getTotalDemand()/instance.N_trucks;
    double penalty=0.0;
    if(route.total_demand() > average){
        double alpha = 1;
        penalty = pow((route.total_demand()-average)/(alpha*average),2);
    }
    if (!sorted_clients.empty()) {
        vector<Client*> candidates;
        int count = min(3, (int)sorted_clients.size()); // Ensure we don't exceed the size of the vector
        if(route.clients.empty()){
            for (int i = 0; i < sorted_clients.size(); i++)
            {
                if(!sorted_clients[i].second->truck_customer ){
                    candidates.push_back(sorted_clients[i].second); 
                }
                if(candidates.size() == count){
                    break;
                }
            }
        }
        if(candidates.size() < count){
            for (int i = candidates.size(); i < count; ++i) {
                candidates.push_back(sorted_clients[i].second);
            }
        }
        // Shuffle the first 5 elements
        shuffle(candidates.begin(), candidates.begin() + count, default_random_engine(random_device()()));
            // 20 % of the time, return the trailer location


        // if(!route.trailer_attached){
        //     // 20 % of the time, return the trailer location
        //     if (rand() % 100 < 80) {
            
        //         return nullptr;
        //     }else{
        //         return candidates[0];
        //     }
            
        // }
        if (rand() % 100 < penalty*100) {
        
            return nullptr;
        }else{
            return candidates[0];
        }
    
        
        // Return one of the shuffled candidates
        return candidates[0];
        
    }

    return nullptr;

}

void Solution::simple_greedy(Instance& instance) {
    precompute_distances(instance);
    // Create trucks and trailers
    std::vector<Truck*> trucks(instance.N_trucks, nullptr);
    std::vector<Trailer*> trailers(instance.N_trailers, nullptr);
    for (int i = 0; i < instance.N_trucks; ++i) {
        trucks[i] = new Truck(instance.truck_capacity);
    }
    for (int i = 0; i < instance.N_trailers; ++i) {
        trailers[i] = new Trailer(instance.trailer_capacity);
    }
    // Assign clients to routes
    for (int i = 0; i < instance.N_trucks; ++i) {
        Truck* truck = trucks[i];
        Trailer* trailer = (i < instance.N_trailers) ? trailers[i] : nullptr;
        Route route(truck, trailer);
        while (route.current_trailer_capacity + route.current_truck_capacity > 0) {
            Client* nearest_client = select_next_client(instance, route);
            if (nearest_client == nullptr) {
                if(!route.trailer_attached && !route.clients.empty() && route.trailer_location!=nullptr){
                    route.add_client(route.trailer_location, instance);
                }else{
                    break;;
                }
            }else{
                route.add_client(nearest_client, instance);
                assigned_clients.insert(nearest_client);
            }     
        }
        route.add_client(&instance.deposit, instance);
        routes.push_back(route);
        }
    }

void Solution::printSolution(){
    cout<<"Feasible: "<<(isFeasible ? "true" : "false")<<endl;
    cout<<"Number of routes: "<<routes.size()<<endl;
    for (Route route : routes) {
        cout << "Route: " << endl;
        cout << "   Truck capacity: " << route.truck->capacity << endl;
        cout << "   Trailer capacity: " << (route.trailer ? route.trailer->capacity : -1) << endl;
        cout << "   Trailer attached: " << route.trailer_attached << endl;
        cout << "   Current capacity: " << route.get_current_capacity() << endl;
        cout << "   Current truck capacity: " << route.current_truck_capacity << endl;
        cout << "   Current trailer capacity: " << route.current_trailer_capacity << endl;
        cout << "   Trailer location: " << route.trailer_location << endl;
        cout << "   Clients: ";
        for (Client* client : route.clients) {
            cout << " " << client->number << " - ";
        }
        cout<<endl;
    }
}

void Solution::exportSolution(Instance& instance, const std::string& filename, const std::string& solutionType, int runNumber) {
    // Export the solution to a file
    size_t pos = filename.find_last_of(".");
    size_t pos2 = filename.find_last_of("/");

    std::string filename2 = filename.substr(pos2+1, pos-pos2-1);

    cout<<"Filename: "<<filename2<<endl;
    std::string base_filename = filename.substr(0, pos) + "_" + solutionType + "_run" + std::to_string(runNumber) + ".txt";
    if(pos2 != std::string::npos){
        base_filename = base_filename.substr(pos2+1);
    }
    std::string full_filename = "solutionsMedium/"+ filename2 + "/"+solutionType +"/"+ base_filename;
    cout<<"Exporting solution to: "<<full_filename<<endl;
    std::ofstream file(full_filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing" << std::endl;
        return;
    }       

    // Write the routes
    for (Route& route : routes) {
        // Write the clients in the route
        for (Client* client : route.clients) {
            if(client->number != instance.deposit.number){
                file << client->number << " ";
            }

        }
        file << std::endl;
    }
    file << "Evaluación: " << evaluate(instance) << std::endl;

    file.close();
}
