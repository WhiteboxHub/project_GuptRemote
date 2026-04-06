# Gupt Remote Desktop (Native Cross-Platform)

Gupt Remote Desktop is a lightweight, high-performance remote desktop application for **Windows** and **macOS**. It uses native system APIs (Win32 & ScreenCaptureKit) for zero-dependency remote access.

---

## 📁 Repository Structure

- **`project-gupt-windows-app/`**: Full C++ source for the Windows version (Win32/GDI).
- **`project-gupt-mac-app/`**: Full C++ source for the macOS version (ScreenCaptureKit/CoreGraphics).
- **`Shared/`**: Common protocol header used by both platforms.

---

## 🚀 Get Started (macOS)

### 1. Build the App
```bash
cd project-gupt-mac-app
mkdir -p build && cd build
rm -rf *
cmake ..
make
```

### 2. Mandatory Security Setup (Important!)
MacOS blocks remote access by default. You **MUST** enable these manually for the app to see your screen:
1. Open **System Settings > Privacy & Security > Screen & System Audio Recording**.
2. Click **+** and add **Terminal** (and/or `GuptMac`) and turn it **ON**.
3. Go to **Privacy & Security > Accessibility**.
4. Click **+** and add **Terminal** and turn it **ON**.

### 3. Run the App
```bash
./GuptMac.app/Contents/MacOS/GuptMac
```

---

## 🚀 Get Started (Windows)

1. Open `project-gupt-windows-app/build/Release`
2. Run **`Gupt.exe`**
3. Select **Client Mode** to control a Mac, or **Host Mode** to be controlled.

---

## 🔗 Cross-Platform Compatibility
- ✅ **Windows to Mac** (Remote Control)
- ✅ **Mac to Windows** (Remote Control)
- ✅ **Mac to Mac** (Remote Control)

---

## 🛠️ Build Requirements
- **Windows:** CMake 3.20+, Visual Studio 2019+
- **macOS:** CMake 3.20+, Xcode Command Line Tools
