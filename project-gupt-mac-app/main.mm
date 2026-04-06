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

    // In a manual memory environment, we use 'this' or 'self' directly as AppDelegate is the singleton app.
    server->SetMessageCallback([self](gupt::shared::MessageType type, const std::vector<uint8_t>& payload) {
        if (type == gupt::shared::MessageType::ConnectRequest) {
            NSLog(@"[DEBUG] Received connection request");
            dispatch_async(dispatch_get_main_queue(), ^{
                NSAlert *alert = [[NSAlert alloc] init];
                alert.messageText = @"Connection Request";
                alert.informativeText = @"A remote peer wants to connect to your Mac. Allow?";
                [alert addButtonWithTitle:@"Allow"];
                [alert addButtonWithTitle:@"Deny"];
                if ([alert runModal] == NSAlertFirstButtonReturn) {
                    gupt::shared::ConnectResponse res;
                    res.accepted = true;
                    std::strncpy(res.reason, "Welcome", sizeof(res.reason));
                    self->server->SendRaw(gupt::shared::SerializeMessage(gupt::shared::MessageType::ConnectResponse, res));
                    self->sessionActive = true;
                    NSLog(@"[DEBUG] Session accepted");
                } else {
                    gupt::shared::ConnectResponse res;
                    res.accepted = false;
                    std::strncpy(res.reason, "User Denied", sizeof(res.reason));
                    self->server->SendRaw(gupt::shared::SerializeMessage(gupt::shared::MessageType::ConnectResponse, res));
                    NSLog(@"[DEBUG] Session denied by user");
                }
            });
        } else if (self->sessionActive) {
            if (type == gupt::shared::MessageType::MouseEvent && payload.size() >= sizeof(gupt::shared::MouseEvent)) {
                auto ev = (const gupt::shared::MouseEvent*)payload.data();
                self->injector.IngestMouseEvent(*ev);
            } else if (type == gupt::shared::MessageType::KeyboardEvent && payload.size() >= sizeof(gupt::shared::KeyboardEvent)) {
                auto ev = (const gupt::shared::KeyboardEvent*)payload.data();
                self->injector.IngestKeyboardEvent(*ev);
            }
        }
    });

    server->Start();
    NSLog(@"[DEBUG] Host server started on port 8080");
    
    std::thread([self]() {
        while (true) {
            if (sessionActive) {
                std::vector<uint8_t> jpeg;
                uint32_t w, h;
                if (capturer.CaptureNextFrameJpeg(jpeg, w, h, 80)) {
                    gupt::shared::FrameDataHeader header{0, w, h, 32, false, 0};
                    server->SendRaw(gupt::shared::SerializeFrame(header, jpeg));
                } else {
                    NSLog(@"[DEBUG] Failed to capture frame (Check Screen Recording Permissions!)");
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    }).detach();
}

- (void)runClientMode:(NSString*)ip {
    client = new gupt::core::network::TcpClient();
    client->SetMessageCallback([self](gupt::shared::MessageType type, const std::vector<uint8_t>& payload) {
        if (type == gupt::shared::MessageType::ConnectResponse && payload.size() >= sizeof(gupt::shared::ConnectResponse)) {
             auto res = (const gupt::shared::ConnectResponse*)payload.data();
             NSLog(@"[DEBUG] Connection response: accepted=%d", res->accepted);
        } else if (type == gupt::shared::MessageType::FrameData) {
            size_t off = sizeof(gupt::shared::FrameDataHeader);
            if (payload.size() <= off) return;
            
            NSData *data = [NSData dataWithBytes:payload.data() + off length:payload.size() - off];
            NSImage *image = [[NSImage alloc] initWithData:data];
            if (image) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    self.remoteView.latestImage = image;
                    [self.remoteView setNeedsDisplay:YES];
                });
            }
        }
    });

    NSLog(@"[DEBUG] Client connecting to %@", ip);
    if (client->Connect([ip UTF8String], 8080)) {
        gupt::shared::ConnectRequest req;
        std::strncpy(req.sessionId, "session", sizeof(req.sessionId));
        std::strncpy(req.authenticationToken, "token", sizeof(req.authenticationToken));
        client->SendRaw(gupt::shared::SerializeMessage(gupt::shared::MessageType::ConnectRequest, req));
        NSLog(@"[DEBUG] Connection request sent");
    } else {
        NSLog(@"[ERROR] Failed to connect to %@", ip);
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
        input.stringValue = @"127.0.0.1";
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
