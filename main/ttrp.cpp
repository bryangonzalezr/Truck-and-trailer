#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <tuple>
#include <random>
#include <limits>

#include "Client.h"
#include "Truck.h"
#include "Trailer.h"
#include "Instance.h"
#include "Route.h"
#include "Solution.h"

using namespace std;

void parse_instance(const std::string& filename, Instance& instance) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file >> instance.N_trucks >> instance.truck_capacity >> instance.N_trailers >> instance.trailer_capacity >> instance.N_clients;
    file >> instance.deposit.number >> instance.deposit.x >> instance.deposit.y >> instance.deposit.demand >> instance.deposit.truck_customer;

    for (int i = 1; i <= instance.N_clients; ++i) {
        int client_number;
        double x, y, demand;
        bool truck_customer;
        file >> client_number >> x >> y >> demand >> truck_customer;
        instance.clients.push_back(new Client(client_number, x, y, demand, truck_customer));
    }

    file.close();
}

void printInstance(Instance instance) {
    cout << "Number of trucks: " << instance.N_trucks << endl;
    cout << "Truck capacity: " << instance.truck_capacity << endl;
    cout << "Number of trailers: " << instance.N_trailers << endl;
    cout << "Trailer capacity: " << instance.trailer_capacity << endl;
    cout << "Number of clients: " << instance.N_clients << endl;
    cout << "Deposit: " << endl;
    cout << "   X: " << instance.deposit.x << endl;
    cout << "   Y: " << instance.deposit.y << endl;
    cout << "   Demand: " << instance.deposit.demand << endl;
    cout << "   Truck customer: " << instance.deposit.truck_customer << endl;

    cout << "Clients: " << endl;
    for (Client* client : instance.clients) {
        cout << "   Client number: " << client->number << endl;
        cout << "       X: " << client->x << endl;
        cout << "       Y: " << client->y << endl;
        cout << "       Demand: " << client->demand << endl;
        cout << "       Truck customer: " << client->truck_customer << endl;
    }

    cout << "Total demand: " << instance.getTotalDemand() << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <instance_file> <num_iterations> <solution_type> <run_number>" << std::endl;
        return 1;
    }
    
    std::string instance_file = argv[1];
    int num_iterations = std::stoi(argv[2]);
    std::string solution_type = argv[3];
    int run_number = std::stoi(argv[4]);

    double best_distance = std::numeric_limits<double>::max();
    Solution best_solution;
    double total_time = 0.0;
    auto start_time = std::chrono::steady_clock::now();
    Instance instance(0, 0.0, 0, 0.0, std::vector<Client*>());
    parse_instance(instance_file, instance);

    for (int i = 0; i < num_iterations; ++i) {
        Solution solution;
        solution.simple_greedy(instance);

        double distance = solution.evaluate(instance);
        if (distance <= best_distance) {
            best_distance = distance;
            best_solution = solution;
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "Best solution: ";
    best_solution.printSolution();
    std::cout << "Total distance:" << best_distance << std::endl;
    std::cout << "Execution time:" << total_time << " ms" << std::endl;


    best_solution.exportSolution(instance, instance_file, solution_type, run_number);

    for (Client* client : best_solution.assigned_clients) {
        delete client;
    }

    return 0;
}
