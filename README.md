# The Backrooms: 3D Procedural Labyrinth

<p align="center">
  <img src="https://github.com/user-attachments/assets/84152c49-f7be-4ebc-8b21-b34d4d356cf6" width="32%" alt="Gameplay 1" />
  <img src="https://github.com/user-attachments/assets/774aa4fc-ec22-474d-98bd-448d434a73ad" width="32%" alt="Gameplay 2" />
  <img src="https://github.com/user-attachments/assets/847eca4a-748e-4dd1-8397-0407141b9076" width="32%" alt="Gameplay 3" />
</p>

A 3D exploration game inspired by the popular internet legend "The Backrooms," developed as a university semester project. The project features an endless, procedurally generated maze that captures the oppressive, liminal atmosphere of yellow wallpaper and buzzing fluorescent lights.

## Features

* **Procedural Chunk Generation:** The maze levels are dynamically created in real-time as the player navigates the environment, ensuring an infinite and unpredictable spatial experience.
* **Procedural Object Placement:** All items and environmental objects are procedurally generated and intelligently placed relative to the labyrinth's layout.
* **Dynamic Lighting:** Custom lighting implementation designed to emphasize the eerie, empty office space aesthetic of the original creepypasta.
* **Collision Detection:** Fully implemented physics/collision mechanics that process player interactions with walls and objects, keeping the player confined within the generated corridors.

## Tech Stack

* **Language:** C++
* **Graphics API:** OpenGL
* **Environment:** Microsoft Visual Studio

## Getting Started

### Prerequisites

* Microsoft Visual Studio (with Desktop development with C++ workload installed)
* Required OpenGL libraries (e.g., GLFW, GLAD/GLEW) configured in your environment

### Build & Run

1. Clone the repository:
   ```bash
   git clone [your-repository-link]
   ```
2. Navigate to the project folder and open the `.sln` (Solution) file using Visual Studio.
3. Set the project as the Startup Project.
4. Build and run the project (F5 or Ctrl+F5).

## Controls

* **W, A, S, D** — Movement
* **Mouse** — Look around (Camera rotation)
* **Esc** — Exit game

## Project Status

Developed as a semester project for a Computer Graphics course. It serves as a demonstration of procedural generation algorithms and low-level 3D graphics programming using OpenGL.
