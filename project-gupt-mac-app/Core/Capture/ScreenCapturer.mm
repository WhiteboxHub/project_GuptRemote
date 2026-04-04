#import "ScreenCapturer.h"
#import <ScreenCaptureKit/ScreenCaptureKit.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreImage/CoreImage.h>
#import <QuartzCore/QuartzCore.h>

namespace gupt {
namespace core {
namespace capture {

struct ScreenCapturerImpl {
    SCStream *stream;
    CGImageRef latestImage;
    bool hasNewFrame;
};

ScreenCapturer::ScreenCapturer() : m_impl(new ScreenCapturerImpl{nullptr, nullptr, false}) {}
ScreenCapturer::~ScreenCapturer() {
    ScreenCapturerImpl* impl = (ScreenCapturerImpl*)m_impl;
    if (impl->latestImage) CGImageRelease(impl->latestImage);
    delete impl;
}

bool ScreenCapturer::Initialize() {
    // Basic initialization for ScreenCaptureKit
    return true;
}

bool ScreenCapturer::CaptureNextFrameJpeg(std::vector<uint8_t>& outJpeg, uint32_t& width, uint32_t& height, int quality) {
    // For demo purposes, we'll capture the main display using CGDisplayCreateImage
    CGImageRef image = CGDisplayCreateImage(kCGDirectMainDisplay);
    if (!image) return false;

    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithCGImage:image];
    NSDictionary *props = @{NSImageCompressionFactor: @((float)quality / 100.0f)};
    NSData *data = [rep representationUsingType:NSBitmapImageFileTypeJPEG properties:props];
    
    outJpeg.assign((uint8_t*)data.bytes, (uint8_t*)data.bytes + data.length);
    width = (uint32_t)CGImageGetWidth(image);
    height = (uint32_t)CGImageGetHeight(image);
    
    CGImageRelease(image);
    return true;
}

} // namespace capture
} // namespace core
} // namespace gupt
