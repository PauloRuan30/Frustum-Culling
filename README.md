# Vision Culling Debugger
> An interactive 3D graphics showcase demonstrating real-time Frustum Culling mathematics, built from scratch with C++ and Raylib.

![C++](https://img.shields.io/badge/Language-C++-00599C?style=for-the-badge&logo=c%2B%2B)
![Raylib](https://img.shields.io/badge/Engine-Raylib-FFFFFF?style=for-the-badge)
![Xmake](https://img.shields.io/badge/Build-Xmake-222222?style=for-the-badge)

![Showcase](https://raw.githubusercontent.com/username/repository/branch/path/to/thumbnail.jpg)](https://raw.githubusercontent.com/username/repository/branch/path/to/video.mp4)

## 🎯 Overview
This project visualizes the "invisible" mathematics of 3D rendering. Instead of sending every object in a scene to the GPU, this engine mathematically extracts the 6 planes of the camera's View-Projection matrix and performs real-time intersection tests against Axis-Aligned Bounding Boxes (AABBs). 

Objects outside the camera's vision are culled, optimizing rendering performance.

## ✨ Features
* **Real-Time Frustum Culling:** Implements the positive/negative vertex trick for optimized AABB-to-Plane intersection testing.
* **Frustum Locking:** Freeze the camera's projection matrix in world space to visually inspect the 3D culling pyramid.
* **Picture-in-Picture Minimap:** A secondary top-down orthographic camera renders the scene layout and vision cone to an independent texture.
* **State Visualization:** Color-coded rendering (Green = Inside, Yellow = Intersecting, Red = Culled).
* **Perspective Math Correction:** Accurately visualizes clip-space bounds in world space using inverse matrices and the perspective divide (`W`).

## 🎮 Controls
| Key/Input | Action |
| :--- | :--- |
| `W` `A` `S` `D` | Move Camera |
| `Mouse` | Look Around |
| `SPACE` | Lock/Unlock Frustum Wireframe |

## 🚀 Getting Started

### Prerequisites
You will need [Xmake](https://xmake.io/) installed on your system to build the project. Xmake will automatically fetch Raylib for you.

### Build and Run
1. Clone the repository:
   ```bash
   git clone git@github.com:PauloRuan30/Frustum-Culling.git
   cd Frustum-Culling
   ```

2. Curl the XMake, build it and run:
   (here the [xmake](https://xmake.io/guide/quick-start.html) quick start link if you need)
   ```bash
   curl -fsSL https://xmake.io/shget.text | bash
   xmake build
   xmake run
   ```