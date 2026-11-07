# The Backrooms: 3D Procedural Labyrinth

<p align="center">
  <img src="https://github.com/user-attachments/assets/84152c49-f7be-4ebc-8b21-b34d4d356cf6" width="32%" alt="Gameplay 1" />
  <img src="https://github.com/user-attachments/assets/774aa4fc-ec22-474d-98bd-448d434a73ad" width="32%" alt="Gameplay 2" />
  <img src="https://github.com/user-attachments/assets/847eca4a-748e-4dd1-8397-0407141b9076" width="32%" alt="Gameplay 3" />
</p>

An endless, procedurally generated 3D maze inspired by the "Backrooms" creepypasta: yellow wallpaper,
buzzing fluorescent lamps and empty office rooms. Semester project for the Computer Graphics (PGR)
course, written in C++ on top of OpenGL (~2500 lines).

## Tech stack

| | |
|---|---|
| Language | C++17 |
| Graphics | OpenGL 3.1, GLSL 1.40 |
| Windowing / input | freeGLUT |
| Model loading | Assimp |
| Math | GLM |
| Textures | DevIL (through PGR-framework) |
| Framework | [PGR-framework](https://cent.felk.cvut.cz/courses/PGR/) (CTU FEL course library) |
| Build | Visual Studio 2022+ (`.sln`) or CMake |

## Features

**World generation**
- **Infinite deterministic world.** Every cell is computed from a hash of the seed and its coordinates, so the type of any cell (wall / empty / doorway) is known without loading its chunk.
- **Chunk streaming.** The world is split into 16×16 chunks that are loaded and unloaded around the player (`std::map` + `std::unique_ptr`).
- **One VBO/EBO per chunk.** Only visible faces are generated, including faces on chunk borders (neighbour cells are queried from the global hash). Floor, ceiling and walls are drawn as separate index ranges with different materials.
- **Layout-aware furniture.** Plants and box stacks go into corners. Tables with computers, chairs and dustbins go along walls. Balls and chairs go in open space. Overlaps are rejected by a collision check.
- **Flyweight models.** Each model is loaded once and shared by every prop instance.

**Rendering (`obec.frag`)**
- Flashlight: a spot light with a soft cone edge, distance attenuation and specular highlight
- Up to 20 nearest lamps (sorted by distance to the player), some of them flickering
- Exponential distance fog
- Emissive textures; monitors play an animation from a sprite sheet (`screen.frag`)
- Animated dirt on walls, mapped in world coordinates
- Skybox

**Gameplay**
- An anomaly flies along a parametric curve and faces along its tangent
- Interaction based on the dot product between view direction and object: turn computers on/off, pick up boxes, kick the ball (it bounces with gravity)
- Collisions with walls and props, noclip mode
- Two static CCTV cameras
- Hot-reload of `data/config.txt` at runtime

## Controls

| Key | Action |
|---|---|
| W A S D | Move |
| Mouse | Look around |
| Left mouse button | Interact with the object you are looking at |
| F | Toggle flashlight |
| N | Toggle noclip |
| C | Switch camera: player → CCTV 1 → CCTV 2 |
| R | Reload `data/config.txt` |
| Esc | Lock / unlock the mouse cursor |
| Shift + Esc | Quit |

## Configuration

`data/config.txt` is read at startup and again on <kbd>R</kbd>:

| Key | Default | Meaning |
|---|---|---|
| `WINDOW_WIDTH`, `WINDOW_HEIGHT` | 1280, 720 | Window size (startup only) |
| `PLAYER_SPEED` | 3.5 | Movement speed |
| `MOUSE_SENSITIVITY` | 0.3 | Mouse sensitivity |
| `RENDER_DISTANCE` | 3 | Chunks loaded around the player |
| `FOG_DENSITY` | 0.03 | Fog density |
| `MAZE_SEED` | 94845724 | World seed (startup only) |

## Building

### 1. Get PGR-framework

The project depends on PGR-framework, which is **not included** in this repository. Download it from the
[PGR course page](https://cent.felk.cvut.cz/courses/PGR/) and either

- put it **next to** the same repository where the project is located
- or set the environment variable `PGR_FRAMEWORK_ROOT` to its folder (with a trailing backslash, e.g. `C:\libs\pgr-framework\`).

### 2. Visual Studio (Windows)

```bash
git clone https://github.com/Ulianafi/procedural-maze.git
```

1. Open `procedural-maze.sln`.
2. Select the **Win32** platform (the framework ships 32-bit libraries) and Debug or Release.
3. Press **F5**. The working directory and the path to the framework DLLs are already set in the project.


Shaders and `data/` are loaded by relative paths, so **run the executable from the repository root**.

## Project structure

| File | Contents |
|---|---|
| `main.cpp` | Entry point, GLUT callbacks, scene setup, cameras |
| `Maze.h/.cpp` | `Maze`: world hash, chunk streaming, collisions, interaction, lamp selection |
| `Chunk.h/.cpp` | `Chunk`: grid of one chunk, mesh building, prop placement |
| `Objects.h`, `Object.cpp` | `Object` base class, Assimp `ObjectModel`, `Mesh` |
| `ObjectAnomaly.cpp`, `ObjectSkyBox.cpp` | Anomaly on a parametric curve, skybox |
| `Player.h/.cpp`, `Camera.h/.cpp` | Player movement and collisions, camera |
| `Structures.h/.cpp` | Shared structs, shader loading |
| `Config.h/.cpp` | `config.txt` parser |
| `obec.*`, `screen.*`, `skybox.*` | GLSL shaders: main lighting, monitor screens, skybox |
| `data/` | Models, textures and config |

## Assets

3D models and textures in `data/` come from free online sources and are included only so the
project can be built and run.
