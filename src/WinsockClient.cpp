﻿

#include "WinsockClient.h"

WinsockClient::WinsockClient() : connectSocket(INVALID_SOCKET), initialized(false) {}

WinsockClient::~WinsockClient() {
    cleanup();
}

bool WinsockClient::initialize() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        logError("WSAStartup failed");
        return false;
    }
    initialized = true;
    return true;
}

bool WinsockClient::connectToServer(const std::string& serverAddress, const std::string& port) {
    if (!initialized) {
        std::cerr << "Error: Winsock not initialized. Call initialize() first." << std::endl;
        return false;
    }

    struct addrinfo* result = nullptr, * ptr = nullptr, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_UDP;

    int iResult = getaddrinfo(serverAddress.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0) {
        logError("getaddrinfo failed");
        return false;
    }

    for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) {
            freeaddrinfo(result);
            logError("Error creating socket");
            return false;
        }

        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET) {
        std::cerr << "Error: Unable to connect to server" << std::endl;
        return false;
    }

    return true;
}

bool WinsockClient::sendData(const std::string& data) {
    int iResult = send(connectSocket, data.c_str(), static_cast<int>(data.length()), 0);
    if (iResult == SOCKET_ERROR) {
        logError("send failed");
        return false;
    }
    return true;
}

std::string WinsockClient::receiveData() {
    char recvbuf[DEFAULT_BUFLEN];
    int iResult = recv(connectSocket, recvbuf, DEFAULT_BUFLEN, 0);

    if (iResult > 0) {
        return std::string(recvbuf, iResult);
    }
    else if (iResult == 0) {
        std::cerr << "Connection closed by server" << std::endl;
    }
    else {
        logError("recv failed");
    }
    return "";
}

void WinsockClient::disconnect() {
    int iResult = shutdown(connectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        logError("shutdown failed");
    }
    cleanup();
}

void WinsockClient::cleanup() {
    if (connectSocket != INVALID_SOCKET) {
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
    }
    if (initialized) {
        WSACleanup();
        initialized = false;
    }
}

void WinsockClient::logError(const std::string& message) {
    std::cerr << "Error: " << message << " (WSA Error: " << WSAGetLastError() << ")" << std::endl;
}