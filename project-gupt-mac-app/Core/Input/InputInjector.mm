#import "InputInjector.h"
#import <CoreGraphics/CoreGraphics.h>
#import <AppKit/AppKit.h>

namespace gupt {
namespace core {
namespace input {

InputInjector::InputInjector() : m_ScreenWidth(0), m_ScreenHeight(0) {}
InputInjector::~InputInjector() {}

bool InputInjector::Initialize() {
    NSScreen *primaryScreen = [[NSScreen screens] firstObject];
    if (primaryScreen) {
        m_ScreenWidth = primaryScreen.frame.size.width;
        m_ScreenHeight = primaryScreen.frame.size.height;
        return true;
    }
    return false;
}

void InputInjector::IngestMouseEvent(const gupt::shared::MouseEvent& ev) {
    CGPoint point = CGPointMake(ev.normalizedX * m_ScreenWidth, ev.normalizedY * m_ScreenHeight);
    
    CGEventType type = kCGEventNull;
    CGMouseButton button = kCGMouseButtonLeft;
    
    if (ev.buttonId == 1) { // Left
        type = ev.isDown ? kCGEventLeftMouseDown : kCGEventLeftMouseUp;
        button = kCGMouseButtonLeft;
    } else if (ev.buttonId == 2) { // Right
        type = ev.isDown ? kCGEventRightMouseDown : kCGEventRightMouseUp;
        button = kCGMouseButtonRight;
    } else {
        type = kCGEventMouseMoved;
    }

    CGEventRef event = CGEventCreateMouseEvent(NULL, type, point, button);
    if (event) {
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
    }
}

void InputInjector::IngestKeyboardEvent(const gupt::shared::KeyboardEvent& ev) {
    CGEventRef event = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)ev.virtualKeyCode, ev.isDown);
    if (event) {
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
    }
}

} // namespace input
} // namespace core
} // namespace gupt
