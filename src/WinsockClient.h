
#ifndef WINSOCKCLIENT_H
#define WINSOCKCLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <stdexcept>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

class WinsockClient {

public:
    WinsockClient();
    ~WinsockClient();

    bool initialize();
    bool connectToServer(const std::string& serverAddress, const std::string& port);

    bool sendData(const std::string& data);
    std::string receiveData();
    void disconnect();

private:
    void cleanup();
    void logError(const std::string& message);

private:
    SOCKET connectSocket;
    static constexpr int DEFAULT_BUFLEN = 512;
    bool initialized;

};



#endif //WINSOCKCLIENT_H
