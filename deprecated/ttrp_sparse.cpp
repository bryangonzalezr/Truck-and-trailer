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

using namespace std;


class Client {
public:
    int number;         // Client number
    double x;              // X coordinate
    double y;              // Y coordinate
    double demand;         // Demand of the client
    bool truck_customer;// True if the client is a truck customer

    // Constructor
    Client() {}
    Client(int number, double x, double y, double demand, bool truck_customer) : number(number), x(x), y(y), demand(demand), truck_customer(truck_customer) {}

    double distance_to(Client* other) {
        return sqrt(pow(x - other->x, 2) + pow(y - other->y, 2));
    }
    
};

class Truck {
public:
    double capacity;              // Capacity of the truck
    std::vector<Client*> route;  // Route of the truck

    // Constructor
    Truck(double capacity) : capacity(capacity) {}

    // Method to calculate total demand
    int total_demand() {
        int total = 0;
        for (Client* client : route) {
            total += client->demand;
        }
        return total;
    }
};


class Trailer {
    public:
        double capacity;              // Capacity of the trailer

        // Constructor
        Trailer(int capacity) : capacity(capacity) {}
};

class Instance {
    public:
        int N_trucks;           // Number of trucks
        double truck_capacity;  // Capacity of the trucks
        int N_trailers;         // Number of trailers
        double trailer_capacity;// Capacity of the trailers
        std::vector<Client*> clients; // Clients in the instance
        int N_clients;          // Number of clients
        Client deposit;         // Deposit client
        int Total_demand;       // Total demand of the clients

        // Constructor
        Instance(int N_trucks, double truck_capacity, int N_trailers, double trailer_capacity, std::vector<Client*> clients)
                : N_trucks(N_trucks), truck_capacity(truck_capacity), N_trailers(N_trailers), trailer_capacity(trailer_capacity), clients(clients), N_clients(clients.size()) {}        // Method to create a solution
        
        int getTotalDemand(){
            int total = 0;
            for (Client* client : clients) {
                total += client->demand;
            }
            return total;
        }
};

class Route {
public:
    Truck* truck;                     // Truck for this route
    Trailer* trailer;                 // Trailer attached to the truck
    std::vector<Client*> clients;     // Clients served by this route
    bool trailer_attached;            // Flag indicating whether trailer is attached to the truck
    int current_truck_capacity;             // Current capacity of the truck
    int current_trailer_capacity;           // Current capacity of the trailer
    int current_capacity;               // Current capacity of the route
    Client* trailer_location;         // Current location of the trailer

    // Constructor
    Route(Truck* truck, Trailer* trailer = nullptr) : truck(truck), trailer(trailer), trailer_attached(trailer != nullptr ? true : false),
        current_truck_capacity(truck->capacity), current_trailer_capacity(trailer != nullptr ? trailer->capacity : 0),
        current_capacity(current_truck_capacity+ current_trailer_capacity), trailer_location(nullptr) {
        // Add the deposit client to the route
        Client deposit;
        current_capacity -= deposit.demand;
        
    }

    // Method to add a client to the route
    void add_client(Client* client, Instance& instance) {
        
        
        // Add the client to the route
        if(!trailer_attached && (trailer_location != nullptr) && (trailer_location->number == client->number)){
            // cout<<"Attaching trailer"<<endl;
            trailer_attached = true;
            trailer_location = nullptr;
            if(current_truck_capacity <= current_trailer_capacity- instance.trailer_capacity){
                current_truck_capacity += current_trailer_capacity- instance.trailer_capacity;
                current_trailer_capacity = instance.trailer_capacity;
            }else{
                current_trailer_capacity = current_truck_capacity;
                current_truck_capacity = instance.truck_capacity;
            }
        }else{
            // If the next client is a truck-only customer and the trailer is attached, detach the trailer
            if (client->truck_customer && trailer_attached) {
                // cout << "Detaching trailer" << endl;
            // Detach trailer
                trailer_attached = false;
                if(clients.empty()){
                    trailer_location = nullptr;
                }else{
                    trailer_location = clients.back(); // Update trailer location
                }
            }
            if(!trailer_attached){
                current_truck_capacity -= client->demand;
            }else{
                if(current_trailer_capacity >= client->demand){
                    current_trailer_capacity -= client->demand;
                }else{
                    current_truck_capacity -= (client->demand - current_trailer_capacity);
                    current_trailer_capacity = 0;                  
                }
            }
            current_capacity -= client->demand;
        }
        clients.push_back(client);
        
    }

    // Method to calculate total distance traveled in this route
    // You'll need some method to calculate distance between clients
    double total_distance(Client& deposit){
        double total = 0;
        total += clients[0]->distance_to(&deposit);
        for (int i = 0; i < clients.size() - 1; ++i) {
            total += clients[i]->distance_to(clients[i + 1]);
        }
        return total;
    }

    // Method to calculate total demand in this route
    int total_demand() {
        int total = 0;
        for (Client* client : clients) {
            total += client->demand;
        }
        return total;
    }

    int get_current_capacity(){
        if(trailer_attached){
            return current_trailer_capacity+current_truck_capacity;
        }
        return current_truck_capacity;
        
    }
    
};


class Solution {
public:
    std::vector<Route> routes;
    std::unordered_set<Client*> assigned_clients;
    std::unordered_map<int, std::unordered_map<int, double>> distances;

    void construct_routes(std::vector<Client*>& clients, Truck* truck, Trailer* trailer) {
        // Initialize routes and other variables
        return;
       
    }
    void precompute_distances(Instance& instance) {
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
    
    double evaluate(Instance& instance) {
        double total_distance = 0;
        for (Route& route : routes) {
            total_distance += route.total_distance(instance.deposit);
        }
        //check if all instance.clients are on route.assigned_clients
        for (Client* client : instance.clients) {
            if (assigned_clients.count(client) == 0) {
                total_distance = std::numeric_limits<double>::max();
                break;
            }
        }

        return total_distance;
        
    }

    Client* select_next_client(Instance& instance, Route& route) {
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

    void simple_greedy(Instance& instance) {
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
    

    //print current solution
    void printSolution(){
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

    void exportSolution(Instance& instance, string filename){
        // Export the solution to a file
        //process filename
        size_t pos = filename.find_last_of(".");
        size_t pos2 = filename.find_last_of("/");
        filename = filename.substr(0, pos) + "_solution.txt";
        if(pos2 != string::npos){
            filename = filename.substr(pos2+1);
        }
        filename = "solutionsMedium/" + filename;

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing" << std::endl;
            return;
        }       

        // Write the routes
        for (Route& route : routes) {
            // Write the clients in the route
            file << instance.deposit.x<<","<<instance.deposit.y<<" ";
            for (Client* client : route.clients) {

                if(client->number != instance.deposit.number){
                    if(client->truck_customer){
                    file << "T";
                    }
                    file << client->x << "," << client->y << " ";
                } 

            }
            file << std::endl;
        }
        file<< "Evaluación: " << evaluate(instance) << std::endl;

        file.close();
    }
   
    


    // Other methods for optimization, evaluation, etc.
};



void parse_instance(const std::string& filename, Instance& instance) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Read the first row containing the number of trucks, truck capacity, number of trailers, trailer capacity, and number of clients
    file >> instance.N_trucks >> instance.truck_capacity >> instance.N_trailers >> instance.trailer_capacity >> instance.N_clients;

    file >> instance.deposit.number >> instance.deposit.x >> instance.deposit.y >> instance.deposit.demand >> instance.deposit.truck_customer;

    // Read remaining client information from the file and create Client objects
    for (int i = 1; i <= instance.N_clients; ++i) {
        int client_number;
        double x, y, demand;
        bool truck_customer;
        file >> client_number >> x >> y >> demand >> truck_customer;
        instance.clients.push_back(new Client(client_number, x, y, demand, truck_customer));

    }

    file.close();
}

void printInstance(Instance instance){
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

}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <instance_file> <num_runs>" << std::endl;
        return 1;
    }
    
    std::string instance_file = argv[1];
    int num_runs = std::stoi(argv[2]);

    // Define variables to store the best solution information
    double best_distance = std::numeric_limits<double>::max();
    Solution best_solution;
    double total_time = 0.0;
    auto start_time = std::chrono::steady_clock::now();
    Instance instance(0, 0.0, 0, 0.0, std::vector<Client*>());
    parse_instance(instance_file, instance);

    for (int i = 0; i < num_runs; ++i) {


        // Create a solution
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

    // cout << "Instance: " << endl;
    // Print the best solution
    std::cout << "Best solution: ";
    best_solution.printSolution();
    std::cout << "Total distance:" << best_distance << std::endl;
    std::cout << "Execution time:" << total_time<< " ms" << std::endl;

    best_solution.exportSolution(instance,instance_file);

    // Clean up memory
    for (Client* client : best_solution.assigned_clients) {
        delete client;
    }

    return 0;
}