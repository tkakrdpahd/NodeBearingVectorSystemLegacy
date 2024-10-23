/* SocketServer.cpp
 * Security: Confidential
 * Author: Minseok Doo
 * Date: Oct 20, 2024
 * 
 * Purpose of Class
 * 
 */

#include "SocketServer.h"
#include "YamlConverter.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>

SocketServer::SocketServer(int serverPort, AttributesManager& attrManager)
    : serverPort(serverPort), serverSocketFd(-1), attributesManager_(attrManager) {
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);
}

SocketServer::~SocketServer() {
    closeServer();
}

bool SocketServer::startServer() {
    serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFd < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return false;
    }

    if (bind(serverSocketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed." << std::endl;
        return false;
    }

    if (listen(serverSocketFd, 3) < 0) {
        std::cerr << "Listen failed." << std::endl;
        return false;
    }

    std::cout << "Server started and listening on port " << serverPort << std::endl;
    return true;
}

void SocketServer::listenForClients() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    while (true) {
        int clientSocket = accept(serverSocketFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }
        std::cout << "Client connected." << std::endl;

        std::thread([this, clientSocket]() {
            char buffer[1024] = {0};
            while (true) {
                int bytesRead = read(clientSocket, buffer, 1024);
                if (bytesRead <= 0) {
                    std::cerr << "Client disconnected or error occurred." << std::endl;
                    close(clientSocket);
                    break;
                }
                std::cout << "Received: " << buffer << std::endl;

                if (std::string(buffer) == "call_attributes_manager") {
                    // 기존 AttributesManager를 사용
                    std::string response = YamlConverter().ToString(attributesManager_);
                    sendResponse(clientSocket, response);
                } else {
                    sendResponse(clientSocket, "Unknown command received.");
                }
            }
        }).detach();
    }
}

void SocketServer::sendResponse(int clientSocket, const std::string& message) {
    send(clientSocket, message.c_str(), message.size(), 0);
}

void SocketServer::closeServer() {
    if (serverSocketFd >= 0) {
        close(serverSocketFd);
        serverSocketFd = -1;
        std::cout << "Server closed." << std::endl;
    }
}
