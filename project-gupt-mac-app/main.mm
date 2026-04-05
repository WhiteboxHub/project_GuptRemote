#import <Cocoa/Cocoa.h>
#import "Core/Capture/ScreenCapturer.h"
#import "Core/Input/InputInjector.h"
#import "Core/Network/TcpNetwork.h"
#import <iostream>
#import <thread>
#import <mutex>

// --- Client View for Displaying Remote Screen ---
@interface RemoteView : NSView
@property (strong) NSImage *latestImage;
@end

@implementation RemoteView
- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    if (self.latestImage) {
        [self.latestImage drawInRect:self.bounds];
    }
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate> {
    gupt::core::capture::ScreenCapturer capturer;
    gupt::core::input::InputInjector injector;
    gupt::core::network::TcpServer *server;
    gupt::core::network::TcpClient *client;
    std::atomic<bool> sessionActive;
}
@property (strong) NSWindow *window;
@property (strong) RemoteView *remoteView;
@end

@implementation AppDelegate

- (void)runHostMode {
    server = new gupt::core::network::TcpServer(8080);
    injector.Initialize();
    capturer.Initialize();

    server->SetMessageCallback([&](gupt::shared::MessageType type, const std::vector<uint8_t>& payload) {
        if (type == gupt::shared::MessageType::ConnectRequest) {
            dispatch_async(dispatch_get_main_queue(), ^{
                NSAlert *alert = [[NSAlert alloc] init];
                alert.messageText = @"Connection Request";
                alert.informativeText = @"A remote peer wants to connect to your Mac. Allow?";
                [alert addButtonWithTitle:@"Allow"];
                [alert addButtonWithTitle:@"Deny"];
                if ([alert runModal] == NSAlertFirstButtonReturn) {
                    gupt::shared::ConnectResponse res{true, "Welcome"};
                    server->SendRaw(gupt::shared::SerializeMessage(gupt::shared::MessageType::ConnectResponse, res));
                    sessionActive = true;
                } else {
                    gupt::shared::ConnectResponse res{false, "User Denied"};
                    server->SendRaw(gupt::shared::SerializeMessage(gupt::shared::MessageType::ConnectResponse, res));
                }
            });
        } else if (sessionActive) {
            if (type == gupt::shared::MessageType::MouseEvent) {
                auto ev = (const gupt::shared::MouseEvent*)payload.data();
                injector.IngestMouseEvent(*ev);
            } else if (type == gupt::shared::MessageType::KeyboardEvent) {
                auto ev = (const gupt::shared::KeyboardEvent*)payload.data();
                injector.IngestKeyboardEvent(*ev);
            }
        }
    });

    server->Start();
    
    std::thread([self]() {
        while (true) {
            if (sessionActive) {
                std::vector<uint8_t> jpeg;
                uint32_t w, h;
                if (capturer.CaptureNextFrameJpeg(jpeg, w, h, 80)) {
                    gupt::shared::FrameDataHeader header{0, w, h, 32, false, 0};
                    server->SendRaw(gupt::shared::SerializeFrame(header, jpeg));
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    }).detach();
}

- (void)runClientMode:(NSString*)ip {
    client = new gupt::core::network::TcpClient();
    client->SetMessageCallback([&](gupt::shared::MessageType type, const std::vector<uint8_t>& payload) {
        if (type == gupt::shared::MessageType::FrameData) {
            size_t off = sizeof(gupt::shared::FrameDataHeader);
            NSData *data = [NSData dataWithBytes:payload.data() + off length:payload.size() - off];
            NSImage *image = [[NSImage alloc] initWithData:data];
            dispatch_async(dispatch_get_main_queue(), ^{
                self.remoteView.latestImage = image;
                [self.remoteView setNeedsDisplay:YES];
            });
        }
    });

    if (client->Connect([ip UTF8String], 8080)) {
        gupt::shared::ConnectRequest req{"session", "token"};
        client->SendRaw(gupt::shared::SerializeMessage(gupt::shared::MessageType::ConnectRequest, req));
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSAlert *modeAlert = [[NSAlert alloc] init];
    modeAlert.messageText = @"Select Mode";
    [modeAlert addButtonWithTitle:@"Host"];
    [modeAlert addButtonWithTitle:@"Client"];
    
    NSInteger result = [modeAlert runModal];
    
    if (result == NSAlertFirstButtonReturn) {
        [self runHostMode];
    } else {
        NSTextField *input = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 200, 24)];
        NSAlert *ipAlert = [[NSAlert alloc] init];
        ipAlert.messageText = @"Enter Host IP";
        ipAlert.accessoryView = input;
        [ipAlert addButtonWithTitle:@"Connect"];
        [ipAlert runModal];
        [self runClientMode:input.stringValue];
        
        NSRect frame = NSMakeRect(0, 0, 1280, 720);
        self.window = [[NSWindow alloc] initWithContentRect:frame styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskResizable | NSWindowStyleMaskClosable) backing:NSBackingStoreBuffered defer:NO];
        self.remoteView = [[RemoteView alloc] initWithFrame:frame];
        self.window.contentView = self.remoteView;
        [self.window makeKeyAndOrderFront:nil];
    }
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        
        AppDelegate *delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        
        [app finishLaunching];
        [app run];
        
        return 0;
    }
}
