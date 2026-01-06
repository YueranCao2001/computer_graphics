# 🎨 Computer Graphics Projects

This repository contains two OpenGL-based Computer Graphics coursework projects developed for **Computer Graphics**.  
Both assignments implement core rendering techniques, geometric modelling, transformations, animation, and interactive controls.

---

## 📘 Project Overview

### **🖼 CW1 – 2D Greeting Card (OpenGL + GLUT)**
A colourful interactive 2D greeting-card scene celebrating XJTLU’s anniversary.  
The program showcases polygon rendering, parametric curves, geometric transformation, and animation.

#### **✨ Key Features**
- **Custom background using multicolored polygons**, with gradient achieved by color interpolation  
- **Text rendering** supporting Chinese & English characters  
- **Billboard drawing using GL_FRONT / GL_BACK / GL_FILL / GL_LINE**  
- **Circles & ellipses drawn via parametric equations**  
  - Circle: `x = r cos(360t), y = r sin(360t)`  
  - Ellipse: `x = a cos(360t), y = b sin(360t)`  
- **Clouds, trees, balloons** constructed from ellipses + circles  
- **Rainbow using glutSolidTorus()** (showing half via translation)
- **Geometric transformations:** scaling, translation using `glPushMatrix()` & `glPopMatrix()`

#### **🎮 Keyboard Controls** 
- `q` – Quit program  
- `f` – Toggle between fill mode and line mode  
- `r` – Reset animations  
- `o` – Reverse direction of clouds & balloons  
- `t` – Stop movement  
- `b` / `s` – Scale objects bigger / smaller  
- `c` – Restore original size  

#### **🖱 Mouse Interaction**
- Left click: decelerate movement (if positive direction)  
- Right click: accelerate movement  

---

### **🧱 CW2 – 3D Modelling Project (OpenGL + GLUT)**  
A hierarchical 3D scene including buildings, ground, trees, UFO animation, boat movement, and textured surfaces.  
Demonstrates modelling, lighting, projection, and interactive navigation.

#### **✨ Key Features**
- **Hierarchical modelling**: ground, buildings, components built in structured layers
- **Geometry creation** using cubes, trapezoidal prisms, and glutSolidSphere  
- **Transformations** for positioning, rotation, scaling of models  
- **Orthographic projection** using `glOrtho()`  
- **Lighting & materials**: ambient, diffuse, specular, shininess   
- **Texture mapping** on roofs, grass, carpets 
- **Animation**: UFO rotation/movement, boat navigation  

#### **🎮 Keyboard Controls** 
- `q` – Quit  
- `b` / `l` / `c` – Scale trees bigger / smaller / reset  
- `w`, `a`, `s`, `d`, `h` – Control boat movement  
- `t` / `r` – Stop / reset UFO animation  

#### **⤴️ Special Keys**
- Arrow keys ← ↑ ↓ →: rotate the entire model view  
- `PgUp`, `PgDn`: zoom in / out  

#### **🖱 Mouse Interaction** 
- Left click: slow down if direction is positive, speed up if negative  
- Right click: speed up if direction is positive, slow down if negative  

---

## 📂 Repository Structure

```
computer_graphics/
│
├── CW1/ # 2D Greeting Card project
├── CW2/ # 3D Modelling & Animation project
├── LICENSE
└── .gitattributes
```

---

## 🛠 Build & Run Instructions
Both CW1 and CW2 require:
- OpenGL  
- GLUT / FreeGLUT  
- A C++ compiler supporting OpenGL linking  

General compilation:
```
g++ main.cpp -lopengl32 -lglu32 -lfreeglut -o graphics_project
./graphics_project
```

(Modify filenames accordingly.)

## 🎯 Learning Outcomes

These projects demonstrate:

- Low-level 2D/3D rendering using OpenGL

- Parametric modelling & geometric construction

- Hierarchical and scene-based modelling

- Transformations, lighting, and texture mapping

- Interactive graphics programming


