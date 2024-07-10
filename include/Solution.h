#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "Route.h"
#include "Instance.h"

class Solution {
public:
    bool isFeasible;
    std::vector<Route> routes;
    std::unordered_set<Client*> assigned_clients;
    std::unordered_map<int, std::unordered_map<int, double>> distances;

    void construct_routes(std::vector<Client*>& clients, Truck* truck, Trailer* trailer);
    void precompute_distances(Instance& instance);
    double evaluate(Instance& instance);
    Client* select_next_client(Instance& instance, Route& route);
    void simple_greedy(Instance& instance);
    void printSolution();
    void exportSolution(Instance& instance, const std::string& filename, const std::string& solutionType, int runNumber) ;

};

#endif // SOLUTION_H
