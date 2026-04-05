#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

namespace gupt {
namespace shared {

enum class MessageType : uint8_t {
    ConnectRequest = 1,
    ConnectResponse = 2,
    FrameData = 3,
    MouseEvent = 4,
    KeyboardEvent = 5,
    Heartbeat = 6,
    Disconnect = 7,
    ClipboardData = 8
};

#pragma pack(push, 1)

struct MessageHeader {
    MessageType type;
    uint32_t payloadSize;
};

struct ConnectRequest {
    char sessionId[32];
    char authenticationToken[64];
};

struct ConnectResponse {
    bool accepted;
    char reason[128];
};

struct FrameDataHeader {
    uint32_t frameNumber;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    bool isDelta;
    uint64_t timestampMs;
};

struct MouseEvent {
    float normalizedX;
    float normalizedY;
    uint8_t buttonId;
    bool isDown;
    int32_t wheelDelta;
};

struct KeyboardEvent {
    uint16_t virtualKey;
    bool isDown;
};

#pragma pack(pop)

// Serialization Helpers
template<typename T>
inline std::vector<uint8_t> SerializeMessage(MessageType type, const T& payload) {
    std::vector<uint8_t> buffer(sizeof(MessageHeader) + sizeof(T));
    MessageHeader* header = reinterpret_cast<MessageHeader*>(buffer.data());
    header->type = type;
    header->payloadSize = sizeof(T);
    std::memcpy(buffer.data() + sizeof(MessageHeader), &payload, sizeof(T));
    return buffer;
}

inline std::vector<uint8_t> SerializeFrame(const FrameDataHeader& frameInfo, const std::vector<uint8_t>& frameData) {
    uint32_t payloadSize = sizeof(FrameDataHeader) + (uint32_t)frameData.size();
    std::vector<uint8_t> buffer(sizeof(MessageHeader) + payloadSize);
    MessageHeader* header = reinterpret_cast<MessageHeader*>(buffer.data());
    header->type = MessageType::FrameData;
    header->payloadSize = payloadSize;
    std::memcpy(buffer.data() + sizeof(MessageHeader), &frameInfo, sizeof(FrameDataHeader));
    std::memcpy(buffer.data() + sizeof(MessageHeader) + sizeof(FrameDataHeader), frameData.data(), frameData.size());
    return buffer;
}

} // namespace shared
} // namespace gupt

#endif // PROTOCOL_H
