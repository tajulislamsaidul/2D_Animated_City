# 🌆 2D Animated City Landscape

### *An Interactive OpenGL Computer Graphics Project*

![C++](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)
![OpenGL](https://img.shields.io/badge/Graphics-OpenGL-green.svg)
![GLUT](https://img.shields.io/badge/Library-GLUT-orange.svg)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen.svg)

## 📖 Overview

**2D Animated City Landscape** is an interactive computer graphics project developed using **C++ and OpenGL (GLUT)**.

It showcases fundamental **graphics algorithms**, **2D transformations**, and **real-time animation techniques** by rendering a dynamic city environment featuring buildings, vehicles, weather systems, and user interaction.

## 🚀 Key Highlights

* 🎯 Implementation of classic **Computer Graphics Algorithms**
* 🌦️ Real-time **environment simulation** (day/night, rain)
* 🎮 **User interaction** via keyboard controls
* 🔄 Continuous **animation using timer functions**
* 🏙️ Richly detailed **multi-layered scene design**

## 🧠 Algorithms Implemented

| Algorithm                     | Usage                                  |
| ----------------------------- | -------------------------------------- |
| **DDA Line Algorithm**        | Road markings, waves, lamp poles       |
| **Bresenham Line Algorithm**  | Bridge structure, bird wings, outlines |
| **Midpoint Circle Algorithm** | Wheels, sun/moon, lamp glow            |
| **Basic Primitives**          | Buildings, houses, environment         |
| **2D Transformations**        | Windmill rotation                      |

## 🌍 Scene Composition

### 🏙️ Environment

* Multi-layered city skyline
* Houses, trees, billboard
* Bridge with structural detailing

### 🌊 Natural Elements

* Animated river with wave effects
* Moving clouds and birds
* Day & night sky with stars

### 🚗 Dynamic Objects

* Moving traffic cars
* Player-controlled car
* Boat animation
* Rotating windmill

### 🌧️ Weather System

* Rain simulation with particle system
* Dynamic lighting conditions

## 🎮 Controls

| Key     | Function             |
| ------- | -------------------- |
| `D`     | Enable Day Mode      |
| `N`     | Enable Night Mode    |
| `L`     | Toggle Street Lights |
| `R`     | Toggle Rain          |
| `← / →` | Move Player Car      |
| `ESC`   | Exit Application     |

## 🛠️ Tech Stack

* **Language:** C++
* **Graphics API:** OpenGL
* **Library:** GLUT
* **Concepts:**

  * Computer Graphics Algorithms
  * 2D Transformations
  * Event Handling
  * Real-time Rendering

## 📂 Project Structure

```
📁 Project Root
 ├── 2D city Projects.cpp   # Main source code
 └── README.md              # Documentation
```
## ⚙️ Installation & Execution

### 🔧 Prerequisites

* C++ Compiler (GCC / MinGW / Visual Studio)
* OpenGL & GLUT installed

### ▶️ Run on Linux / macOS

```bash
g++ "2D city Projects.cpp" -o city -lGL -lGLU -lglut
./city
```
### ▶️ Run on Windows (MinGW)

```bash
g++ "2D city Projects.cpp" -o city.exe -lfreeglut -lopengl32 -lglu32
city.exe
```
## 📸 Demo Preview

> ⚠️ *Add screenshots or GIFs here for better presentation*

```
📁 assets/
 ├── screenshot1.png
 ├── screenshot2.png
 └── demo.gif
```

## 📚 Learning Outcomes

This project demonstrates:

* ✔️ Practical implementation of **line & circle drawing algorithms**
* ✔️ Understanding of **rendering pipeline**
* ✔️ Use of **transformations in animation**
* ✔️ Real-time **event-driven programming**
* ✔️ Scene design & graphical composition

## 🔮 Future Enhancements

* 🚦 Traffic system with signals & AI vehicles
* 🖱️ Mouse interaction support
* 🔊 Sound effects integration
* 🎮 Convert into a mini game
* 🌐 Add GUI controls (menus, settings)

## 👨‍💻 Author
📌 Computer Graphics Project

## 📄 License

This project is intended for **educational purposes**.
Feel free to use, modify, and improve it.
