# 🚀 OpenGL Formula 1 Circuit Simulation ([GoogleDrive](https://drive.google.com/drive/folders/1KrqS1WrZiPOAyNeeofEjY-ObvffRisJi?usp=drive_link))

This project implements a **Formula 1 circuit simulation** using **OpenGL**, featuring **dynamic vehicle animation, real-time lighting, and environmental effects**. The scene includes a **detailed racetrack, animated vehicles, lighting effects, and weather conditions**, offering an immersive 3D experience.

---

![Photo](https://github.com/dansimina/OpenGL-project/blob/main/presentation_photo.png)

---

## 📜 Project Overview

### 🏎️ Scene Description
The OpenGL-rendered scene features:
- A **Formula 1 circuit** designed around a lake.
- Natural elements such as **trees and environmental lighting**.
- **Four light poles** placed strategically for optimal visibility.
- A **Ferrari F1 car**, fully animated and following a predefined track.
- **Three garages** near the start/finish line.
- **Weather effects**, including dynamic fog and rain.

---

### 🎮 Functionalities
- **Camera movement** using the keyboard and mouse.
- **Ambient light control** to adjust scene brightness.
- **Fog intensity adjustment** for atmospheric effects.
- **Rain animation toggle** for dynamic weather changes.
- **Camera auto-follow mode** to track the F1 car.
- **Switching between rendering modes** (wireframe/solid view).

---

## 🏗️ Implementation Details

### 🚗 Car Animation Algorithm
The car follows a predefined **Catmull-Rom interpolated path**, ensuring smooth movement and rotations.

#### Key Functions:
1. **`initPoints()`**:
   - Initializes **18 control points**, each with a **3D position** and a rotation angle.
   - These points define the **track trajectory**.

2. **`catmullRomInterpolation()`**:
   - Implements **Catmull-Rom spline interpolation**.
   - Computes **smooth curves** between control points for fluid motion.

3. **`interpolateAngle()`**:
   - Ensures the car **rotates naturally** between two angles.
   - Prevents sudden direction changes by selecting the **shortest rotation path**.

4. **`moveCar()`**:
   - Calculates the next **car position** based on **Catmull-Rom interpolation**.
   - Updates the car’s **position and rotation**.
   - Automatically transitions between **track segments**.

---

### 🔦 Lighting & Shading
The project uses **custom OpenGL shaders** for realistic rendering.

#### Shader Types:
- **Basic Shaders**: Handles **vertex transformations and fragment coloring**.
- **Skybox Shaders**: Simulates an environment background.
- **Light Cube Shaders**: Implements **multiple light sources** for realism.
- **Rear Light Shader**: Simulates a red glow behind the F1 car.
- **Depth Mapping Shader**: Implements **shadow mapping**.
- **Rain Shader**: Generates a **dynamic rain effect**.

---

### 📊 Data Structures
#### 🚗 Car Position Structure:
- Stores **3D coordinates (x, y, z)**.
- Holds the **rotation angle** for smooth movement.

#### 🌧️ Rain Particle Structure:
- Defines **randomized raindrop positions**.
- Animates the **falling motion** of rain particles.

---

## 🎮 User Controls

| Action | Key |
|--------|-----|
| Move Camera | `W, A, S, D` |
| Rotate Scene | `Q, E` |
| Toggle Wireframe Mode | `T` |
| Toggle Solid Mode | `Y` |
| Start/Stop Car Animation | `F` |
| Move Car to Next Position | `N` |
| Toggle Rain Animation | `Z` |
| Toggle Camera Tracking | `V` |
| Manual Car Control | `I, K, J, L, G, H` |
| Rotate Car | `U, O` |
| Adjust Ambient Light | `1, 2` |
| Adjust Fog Effect | `3, 4` |

---

## ⚙️ Running the Project
Ensure you have **OpenGL, GLFW, and GLEW** installed.


