# BitSketch

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

BitSketch is a pixel art editor and image-to-hex converter built with C++ and Qt5. It allows users to create pixel art, edit pixel grids, and convert images to RGB565 hex format for embedded systems.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)
- [Authors](#authors)

---

## Features

### **1. Pixel Art Editor**
- Create and edit pixel art with customizable grid sizes (width & height).
- Drawing tools: pick colors, draw with the left mouse button, erase with the right mouse button.
- Zoom in/out on the grid, display pixel coordinates.
- Undo support (Ctrl+Z).
- Preview designs and save them as PNG or hex files.

### **2. Image to Hex Converter**
- Convert images (PNG, JPG, BMP) to RGB565 hex code.
- Export hex code to TXT files as C/C++ arrays.

### **3. UI**
- Optimized interface with Minimize, Maximize/Restore, and Close buttons.
- Flexible layout for an intuitive user experience.

---

## Prerequisites

- **Qt 5.x**: Including `Widgets`, `Gui`, and `Core` modules.
- **C++11**: Compatible compiler (e.g., GCC, MSVC, Clang).
- **CMake** or **qmake**: Build tools.
- **Operating System**: Linux.

---

## Installation

### **1. Clone Repository**
```bash
git clone https://github.com/Jerguel02/BitSketch-Cpp-Version.git
cd BitSketch-Cpp-Version
```

### **2. Install Qt**
- Download and install [Qt](https://www.qt.io/download) with the required modules.

### **3. Build with CMake**
```bash
mkdir build
cd build
cmake ..
make
```

### **Or build with qmake**
```bash
qmake ../BitSketch.pro
make
```

### **4. Run the Application**
```bash
./BitSketch
```

---

## Usage

### **Launch the Application**
- Run the program to open the main window (`MainWindow`).
- Click "Select Pixel" to open the pixel editor (`PixelArtDialog`).

### **Create Pixel Art**
1. Set grid size (`Width`, `Height`), then click "Apply Size".
2. Pick a color and draw/erase on the grid.
3. Use "Zoom In"/"Zoom Out" to adjust the level of detail.
4. Click "Save Design" to export as PNG or TXT.

### **Convert Images**
1. In the main window, click "Select Image" to load a file.
2. Click "Save Hex" to convert and export hex code.

---

## Project Structure

```
BitSketch-Cpp-Version/
├── CMakeLists.txt       # CMake configuration
├── main.cpp             # Program entry point
├── mainwindow.h/cpp     # Main window and hex converter
├── pixelartdialog.h/cpp # Pixel art editor
├── previewdialog.h/cpp  # Design preview
└── README.md            # This documentation
```

---

## Contributing

1. Fork the repository: [BitSketch-Cpp-Version](https://github.com/Jerguel02/BitSketch-Cpp-Version).
2. Create a new branch:
   ```bash
   git checkout -b feature/your-feature
   ```
3. Commit your changes:
   ```bash
   git commit -m "Add your feature"
   ```
4. Push and open a Pull Request.

Report bugs or suggest improvements in the [Issues](https://github.com/Jerguel02/BitSketch-Cpp-Version/issues) section.

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

## Authors

- **Pham Huu Nghia** - Main Developer - [GitHub Profile](https://github.com/Jerguel02)


