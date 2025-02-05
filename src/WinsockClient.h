
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

    bool isInitialized() const { return _isInitialized; }
    bool isConnected() const { return _isConnected; }

private:
    void cleanup();
    void logError(const std::string& message);

private:
    static constexpr int DEFAULT_BUFLEN = 512;

    SOCKET _connectSocket;
    bool _isInitialized;
    bool _isConnected;

};



#endif //WINSOCKCLIENT_H
