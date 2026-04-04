# Gupt Remote Desktop (Cross-Platform)

Gupt Remote Desktop is a lightweight, high-performance remote desktop application available for **Windows** and **macOS**. It allows you to view and control your computer remotely with zero external dependencies and a tiny footprint.

---

## 🚀 Quick Start (Which machine are you using?)

### 🪟 Windows (PC-A)
The Windows version is already built for you.
- **Project Folder:** `project-gupt-windows-app`
- **How to Run:** Open the `build/Release` folder and double-click **`Gupt.exe`**.
- **Mode:** Select "Host" to share your screen or "Client" to control another PC.

### 🍎 macOS (PC-B)
You must build the Mac version on your Mac computer.
- **Project Folder:** `project-gupt-mac-app`
- **Build Instructions:**
  1. Copy the folder to your Mac.
  2. Open Terminal and run: `mkdir build && cd build && cmake .. && make`.
  3. Run the app: `./GuptMac`.

---

## 🔗 How to Connect

To connect two systems (e.g., Windows and Mac):

1.  **On the HOST (The PC you want to control):**
    - Run the app and select **Host Mode**.
    - Find the IP address using `ipconfig` (Windows) or `ifconfig` (Mac).
2.  **On the CLIENT (The PC you are using to view):**
    - Run the app and select **Client Mode**.
    - Enter the **Host's IP address** and click connect.
3.  **On the HOST:**
    - Click **Yes/Allow** when the security consent dialog appears.

---

## 🛠️ Technical Details

- **Interoperability:** Fully compatible between Windows (Win32 API) and macOS (ScreenCaptureKit).
- **Communication:** Custom TCP binary protocol for low-latency mouse, keyboard, and screen data.
- **Portability:** Self-contained executables; no installer or third-party runtimes required.
- **Privacy:** Always requires user consent on the host side; no silent monitoring possible.

---

## 💻 Advanced Build Instructions (Source)

### Windows
```powershell
cmake -S . -B build
cmake --build build --config Release
```

### macOS
```bash
mkdir build && cd build
cmake ..
make
```
