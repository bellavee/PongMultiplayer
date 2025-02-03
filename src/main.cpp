//
// Created by Bella on 2/2/2025.
//

#include <iostream>

#include "Game.h"
#include "WinsockClient.h"

int main() {
    Game game;
    game.run();

    WinsockClient client;

    if (!client.initialize()) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    if (!client.connectToServer("localhost", "27015")) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }

    if (!client.sendData("Hello, server!")) {
        std::cerr << "Failed to send data" << std::endl;
        return 1;
    }

    std::string response = client.receiveData();
    if (!response.empty()) {
        std::cout << "Received: " << response << std::endl;
    }

    client.disconnect();

    return 0;

}