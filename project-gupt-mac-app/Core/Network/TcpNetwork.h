#ifndef TCP_NETWORK_H
#define TCP_NETWORK_H

#include "../../Shared/Protocol.h"
#include <vector>
#include <string>
#include <functional>
#include <atomic>
#include <thread>

namespace gupt {
namespace core {
namespace network {

class TcpServer {
public:
    TcpServer(int port);
    ~TcpServer();
    void Start();
    void Stop();
    void SendRaw(const std::vector<uint8_t>& data);
    void SetMessageCallback(std::function<void(gupt::shared::MessageType, const std::vector<uint8_t>&)> cb);

private:
    void ListenLoop();
    void ReceiveLoop(int clientSocket);
    
    int m_port;
    int m_serverSocket;
    std::atomic<int> m_clientSocket;
    std::atomic<bool> m_running;
    std::function<void(gupt::shared::MessageType, const std::vector<uint8_t>&)> m_callback;
};

class TcpClient {
public:
    TcpClient();
    ~TcpClient();
    bool Connect(const std::string& ip, int port);
    void Disconnect();
    void SendRaw(const std::vector<uint8_t>& data);
    void SetMessageCallback(std::function<void(gupt::shared::MessageType, const std::vector<uint8_t>&)> cb);

private:
    void ReceiveLoop();
    int m_socket;
    std::atomic<bool> m_connected;
    std::function<void(gupt::shared::MessageType, const std::vector<uint8_t>&)> m_callback;
};

} // namespace network
} // namespace core
} // namespace gupt

#endif // TCP_NETWORK_H
