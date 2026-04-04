# Gupt Remote Desktop (Native Cross-Platform)

Gupt Remote Desktop is a lightweight, high-performance remote desktop application for **Windows** and **macOS**. It uses native system APIs (Win32 & ScreenCaptureKit) for zero-dependency remote access.

---

## 📁 Repository Structure

- **`project-gupt-windows-app/`**: Full C++ source for the Windows version (Win32/GDI).
- **`project-gupt-mac-app/`**: Full C++ source for the macOS version (ScreenCaptureKit/CoreGraphics).

---

## 🚀 Quick Run

### 🪟 Windows
- Open `project-gupt-windows-app/build/Release`
- Run **`Gupt.exe`**

### 🍎 macOS
1. Copy `project-gupt-mac-app` to your Mac.
2. Build it: `mkdir build && cd build && cmake .. && make`.
3. Run: `./GuptMac`.

---

## 🔗 How to Connect

1. **Host (PC you want to control):** Run and select **Host Mode**. Note the IP.
2. **Client (PC you are using):** Run and select **Client Mode**. Enter the Host IP and connect.

---

## 🛠️ Build Requirements

- **Windows:** CMake 3.20+, Visual Studio 2019+
- **macOS:** CMake 3.20+, Xcode (Command Line Tools)
