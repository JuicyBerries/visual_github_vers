# VisualGit

A lightweight, high-performance visual Git client tailored for visual assets and 3D projects (like .vrm avatars and textures). Built with modern C++ and Qt, it aims to provide a seamless "vibe coding" experience without the overhead of Electron-based apps.

## Features

* Visual History: Navigate through your commit log effortlessly.
* Smart UI: Modern, responsive interface built with Qt Widgets.
* Native Performance: Powered by libgit2 (the same C library that powers GitHub's infrastructure) for lightning-fast repository analysis.
* Secure Credentials: Local, encrypted-ready storage for GitHub Personal Access Tokens.
* [WIP] Drag & Drop Staging: Easily stage modified files directly from the UI.
* [WIP] Branch Graph: A custom-rendered commit tree graph.

## Tech Stack

* Language: C++17 / C++20
* Framework: Qt 6 (Widgets)
* Git Engine: libgit2
* Build System: CMake

## Build Instructions (Linux / Ubuntu)

To compile and run this project locally, you will need Qt 6 and libgit2 installed on your system.

### 1. Install Dependencies

Open your terminal and install the required development packages:

```bash
sudo apt update
sudo apt install qt6-base-dev qtcreator cmake ninja-build build-essential libgit2-dev
```

### 2. Clone the Repository
```bash
git clone [https://github.com/your-username/VisualGit.git](https://github.com/your-username/VisualGit.git)
cd VisualGit
```

### 3. Build the Project

You can open the CMakeLists.txt directly in Qt Creator and hit the Run button, or build it manually from the terminal:
```bash
mkdir build && cd build
cmake -G Ninja ..
ninja
```

### 4. Run
```bash
./VisualGit
```

### Contributing

Contributions, issues, and feature requests are welcome! 

