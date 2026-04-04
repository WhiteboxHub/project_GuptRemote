#ifndef SCREEN_CAPTURER_H
#define SCREEN_CAPTURER_H

#include <vector>
#include <cstdint>

namespace gupt {
namespace core {
namespace capture {

class ScreenCapturer {
public:
    ScreenCapturer();
    ~ScreenCapturer();

    bool Initialize();
    bool CaptureNextFrameJpeg(std::vector<uint8_t>& outJpeg, uint32_t& width, uint32_t& height, int quality = 80);

private:
    void* m_impl; // Pointer to Objective-C++ implementation
};

} // namespace capture
} // namespace core
} // namespace gupt

#endif // SCREEN_CAPTURER_H
