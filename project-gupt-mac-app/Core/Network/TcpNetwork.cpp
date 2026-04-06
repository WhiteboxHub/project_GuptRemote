#include "TcpNetwork.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <thread>

namespace gupt {
namespace core {
namespace network {

// --- TcpServer Implementation ---
TcpServer::TcpServer(int port) : m_port(port), m_serverSocket(-1), m_clientSocket(-1), m_running(false) {}
TcpServer::~TcpServer() { Stop(); }

void TcpServer::Start() {
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) return;
    listen(m_serverSocket, 5);
    m_running = true;
    std::thread(&TcpServer::ListenLoop, this).detach();
}

void TcpServer::Stop() {
    m_running = false;
    if (m_serverSocket != -1) close(m_serverSocket);
    if (m_clientSocket != -1) close(m_clientSocket);
    m_serverSocket = -1;
    m_clientSocket = -1;
}

void TcpServer::ListenLoop() {
    while (m_running) {
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int client = accept(m_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        
        if (client != -1) {
            int flag = 1;
            setsockopt(client, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
            
            if (m_clientSocket != -1) close(m_clientSocket);
            m_clientSocket = client;
            std::thread(&TcpServer::ReceiveThread, this).detach();
        }
    }
}

void TcpServer::ReceiveThread() {
    std::vector<uint8_t> buffer(65536);
    std::vector<uint8_t> messageBuffer;
    size_t readPos = 0;
    int client = m_clientSocket;

    while (m_running && m_clientSocket == client) {
        ssize_t bytesReceived = recv(client, buffer.data(), buffer.size(), 0);
        
        if (bytesReceived > 0) {
            messageBuffer.insert(messageBuffer.end(), buffer.begin(), buffer.begin() + bytesReceived);

            while (messageBuffer.size() - readPos >= sizeof(shared::MessageHeader)) {
                shared::MessageHeader* header = reinterpret_cast<shared::MessageHeader*>(messageBuffer.data() + readPos);
                uint32_t totalMessageSize = sizeof(shared::MessageHeader) + header->payloadSize;

                if (totalMessageSize > 150000000) { // 150MB limit
                    close(client);
                    m_clientSocket = -1;
                    return;
                }

                if (messageBuffer.size() - readPos >= totalMessageSize) {
                    std::vector<uint8_t> msgData(messageBuffer.data() + readPos + sizeof(shared::MessageHeader), messageBuffer.data() + readPos + totalMessageSize);
                    
                    if (m_callback) {
                        m_callback(header->type, msgData);
                    }

                    readPos += totalMessageSize;
                } else {
                    break;
                }
            }

            if (readPos > messageBuffer.size() / 2) {
                messageBuffer.erase(messageBuffer.begin(), messageBuffer.begin() + readPos);
                readPos = 0;
            }
        } else {
            close(client);
            if (m_clientSocket == client) m_clientSocket = -1;
            break;
        }
    }
}

void TcpServer::SendRaw(const std::vector<uint8_t>& data) {
    if (m_clientSocket != -1) {
        const uint8_t* ptr = data.data();
        size_t totalSent = 0;
        size_t toSend = data.size();
        while (totalSent < toSend) {
            ssize_t sent = send(m_clientSocket, ptr + totalSent, toSend - totalSent, 0);
            if (sent <= 0) break;
            totalSent += sent;
        }
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
        int flag = 1;
        setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
        m_connected = true;
        std::thread(&TcpClient::ReceiveThread, this).detach();
        return true;
    }
    return false;
}

void TcpClient::Disconnect() {
    m_connected = false;
    if (m_socket != -1) close(m_socket);
    m_socket = -1;
}

void TcpClient::ReceiveThread() {
    std::vector<uint8_t> buffer(65536);
    std::vector<uint8_t> messageBuffer;
    size_t readPos = 0;

    while (m_connected && m_socket != -1) {
        ssize_t bytesReceived = recv(m_socket, buffer.data(), buffer.size(), 0);
        
        if (bytesReceived > 0) {
            messageBuffer.insert(messageBuffer.end(), buffer.begin(), buffer.begin() + bytesReceived);

            while (messageBuffer.size() - readPos >= sizeof(shared::MessageHeader)) {
                shared::MessageHeader* header = reinterpret_cast<shared::MessageHeader*>(messageBuffer.data() + readPos);
                uint32_t totalMessageSize = sizeof(shared::MessageHeader) + header->payloadSize;

                if (totalMessageSize > 150000000) {
                    Disconnect();
                    return;
                }

                if (messageBuffer.size() - readPos >= totalMessageSize) {
                    std::vector<uint8_t> msgData(messageBuffer.data() + readPos + sizeof(shared::MessageHeader), messageBuffer.data() + readPos + totalMessageSize);
                    
                    if (m_callback) {
                        m_callback(header->type, msgData);
                    }

                    readPos += totalMessageSize;
                } else {
                    break;
                }
            }

            if (readPos > messageBuffer.size() / 2) {
                messageBuffer.erase(messageBuffer.begin(), messageBuffer.begin() + readPos);
                readPos = 0;
            }
        } else {
            Disconnect();
            break;
        }
    }
}

void TcpClient::SendRaw(const std::vector<uint8_t>& data) {
    if (m_connected && m_socket != -1) {
        const uint8_t* ptr = data.data();
        size_t totalSent = 0;
        size_t toSend = data.size();
        while (totalSent < toSend) {
            ssize_t sent = send(m_socket, ptr + totalSent, toSend - totalSent, 0);
            if (sent <= 0) break;
            totalSent += sent;
        }
    }
}

void TcpClient::SetMessageCallback(std::function<void(gupt::shared::MessageType, const std::vector<uint8_t>&)> cb) {
    m_callback = cb;
}

} // namespace network
} // namespace core
} // namespace gupt
