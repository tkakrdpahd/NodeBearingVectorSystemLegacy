/* SocketServer.h
 * Linked file SocketServer.cpp
 * Security: Confidential
 * Author: Minseok Doo
 * Date: Oct 20, 2024
 * 
 * Purpose of Class
 * Convert AttributeManager to .yaml
 */

#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <string>
#include <arpa/inet.h>
#include "AttributesManager.h" // AttributesManager 클래스 포함

class SocketServer {
public:
    // 생성자 선언 수정: AttributesManager 참조 추가
    SocketServer(int serverPort, AttributesManager& attrManager);
    ~SocketServer();

    bool startServer();
    void listenForClients();
    void sendResponse(int clientSocket, const std::string& message);
    void closeServer();

private:
    int serverPort;
    int serverSocketFd;
    struct sockaddr_in serverAddr;
    AttributesManager& attributesManager_; // AttributesManager 참조
};

#endif // SOCKETSERVER_H
