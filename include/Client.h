#ifndef CLIENT_H
#define CLIENT_H

class Client {
public:
    int number;         // Client number
    double x;           // X coordinate
    double y;           // Y coordinate
    double demand;      // Demand of the client
    bool truck_customer;// True if the client is a truck customer

    // Constructor
    Client();
    Client(int number, double x, double y, double demand, bool truck_customer);

    double distance_to(Client* other);
};

#endif // CLIENT_H