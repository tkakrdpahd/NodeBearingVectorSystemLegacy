/* SocketServer.h
 * Linked file SocketServer.cpp
 * Security: Confidential
 * Author: Minseok Doo
 * Date: Oct 20, 2024
 * 
 * Purpose of Class
 * Convert AttributeManager to .yaml
 * 
 * Equations
 */
#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <string>
#include <arpa/inet.h>

// GetAttributesManagerData 함수 선언
std::string GetAttributesManagerData();

class SocketServer {
public:
    SocketServer(int serverPort);
    ~SocketServer();

    bool startServer();
    void listenForClients();
    void sendResponse(int clientSocket, const std::string& message);
    void closeServer();

private:
    int serverPort;
    int serverSocketFd;
    struct sockaddr_in serverAddr;
};

#endif // SOCKETSERVER_H
