#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include <string>
#include <cstdint>

namespace gupt {
namespace shared {

enum class MessageType : uint8_t {
    ConnectRequest = 1,
    ConnectResponse = 2,
    FrameData = 3,
    MouseEvent = 4,
    KeyboardEvent = 5,
    ClipboardUpdate = 6,
    Heartbeat = 7
};

struct FrameDataHeader {
    uint32_t frameIndex;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    bool isKeyFrame;
    uint32_t payloadSize;
};

struct MouseEvent {
    float normalizedX;
    float normalizedY;
    uint8_t buttonId; // 0: None, 1: Left, 2: Right, 3: Middle
    bool isDown;
    int16_t wheelDelta;
};

struct KeyboardEvent {
    uint16_t virtualKeyCode;
    bool isDown;
    bool isSystemKey;
};

struct ConnectRequest {
    char sessionId[64];
    char token[64];
};

struct ConnectResponse {
    bool accepted;
    char message[128];
};

// Serialization Helpers
inline std::vector<uint8_t> SerializeMessage(MessageType type, const void* data, size_t size) {
    std::vector<uint8_t> buf;
    buf.reserve(1 + size);
    buf.push_back(static_cast<uint8_t>(type));
    const uint8_t* ptr = static_cast<const uint8_t*>(data);
    buf.insert(buf.end(), ptr, ptr + size);
    return buf;
}

template<typename T>
inline std::vector<uint8_t> SerializeMessage(MessageType type, const T& obj) {
    return SerializeMessage(type, &obj, sizeof(T));
}

inline std::vector<uint8_t> SerializeFrame(const FrameDataHeader& header, const std::vector<uint8_t>& jpegData) {
    std::vector<uint8_t> buf;
    buf.reserve(1 + sizeof(FrameDataHeader) + jpegData.size());
    buf.push_back(static_cast<uint8_t>(MessageType::FrameData));
    const uint8_t* hPtr = reinterpret_cast<const uint8_t*>(&header);
    buf.insert(buf.end(), hPtr, hPtr + sizeof(FrameDataHeader));
    buf.insert(buf.end(), jpegData.begin(), jpegData.end());
    return buf;
}

} // namespace shared
} // namespace gupt

#endif // PROTOCOL_H
