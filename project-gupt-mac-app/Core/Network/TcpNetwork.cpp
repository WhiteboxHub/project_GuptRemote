#include "TcpNetwork.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

namespace gupt {
namespace core {
namespace network {

// --- TcpServer Implementation ---
TcpServer::TcpServer(int port) : m_port(port), m_serverSocket(-1), m_clientSocket(-1), m_running(false) {}
TcpServer::~TcpServer() { Stop(); }

void TcpServer::Start() {
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(m_serverSocket, (struct sockaddr*)&addr, sizeof(addr));
    listen(m_serverSocket, 5);
    m_running = true;
    std::thread(&TcpServer::ListenLoop, this).detach();
}

void TcpServer::Stop() {
    m_running = false;
    if (m_serverSocket != -1) close(m_serverSocket);
    if (m_clientSocket != -1) close(m_clientSocket);
}

void TcpServer::ListenLoop() {
    while (m_running) {
        int client = accept(m_serverSocket, NULL, NULL);
        if (client != -1) {
            m_clientSocket = client;
            ReceiveLoop(client);
        }
    }
}

void TcpServer::ReceiveLoop(int client) {
    while (m_running) {
        uint8_t type;
        if (recv(client, &type, 1, 0) <= 0) break;
        
        uint32_t size;
        if (recv(client, &size, 4, MSG_PEEK) <= 0) break; // Simplified framing for demo
        // ... Proper framing logic here ...
    }
}

void TcpServer::SendRaw(const std::vector<uint8_t>& data) {
    if (m_clientSocket != -1) {
        send(m_clientSocket, data.data(), data.size(), 0);
    }
}

void TcpServer::SetMessageCallback(std::function<void(gupt::shared::MessageType, const std::vector<uint8_t>&)> cb) {
    m_callback = cb;
}

// --- TcpClient Implementation ---
TcpClient::TcpClient() : m_socket(-1), m_connected(false) {}
TcpClient::~TcpClient() { Disconnect(); }

bool TcpClient::Connect(const std::string& ip, int port) {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    if (connect(m_socket, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        m_connected = true;
        std::thread(&TcpClient::ReceiveLoop, this).detach();
        return true;
    }
    return false;
}

void TcpClient::Disconnect() {
    m_connected = false;
    if (m_socket != -1) close(m_socket);
}

void TcpClient::ReceiveLoop() {
    // simplified receive loop
}

void TcpClient::SendRaw(const std::vector<uint8_t>& data) {
    if (m_connected) {
        send(m_socket, data.data(), data.size(), 0);
    }
}

void TcpClient::SetMessageCallback(std::function<void(gupt::shared::MessageType, const std::vector<uint8_t>&)> cb) {
    m_callback = cb;
}

} // namespace network
} // namespace core
} // namespace gupt
