# Gupt Remote Desktop (macOS)

This is the macOS version of **Gupt Remote Desktop**, designed for high-performance screen sharing and remote control. It is fully interoperable with the Windows version.

## 🚀 How to Build

1.  **Ensure Requirements:** You must have **Xcode** and **CMake** installed on your Mac.
2.  **Open Terminal:** Navigate to this folder.
3.  **Build:**
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
4.  **Run:**
    ```bash
    ./GuptMac
    ```

## 🔒 Permissions
On first run, you must grant the following in **System Settings > Privacy & Security**:
- **Screen Recording:** To share your screen.
- **Accessibility:** To allow remote mouse and keyboard control.

## 🔗 Interoperability
- Connect to any Windows PC running **`Gupt.exe`**.
- Connect from any Windows PC to this Mac.
- The binary protocol is exactly the same as the Windows version.
