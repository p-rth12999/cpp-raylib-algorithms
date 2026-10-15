# C++ Raylib Graphics Algorithms

A from-scratch collection of classic Computer Graphics algorithms, implemented in C++ with **raylib**, plus a ready-to-clone project template for starting your own raylib projects on Windows with MinGW.

Every algorithm here computes pixels manually and plots them one by one, instead of relying on raylib's built-in shape functions (`DrawLine`, `DrawCircle`, etc.). The goal is to actually understand each algorithm, not just call a library function that does it for you.

---

## What's in this repo

This repo does three jobs at once:

1. **A graphics-algorithms library** (`algorithms/`): one header + source pair per algorithm, reusable in any raylib project.
2. **Course assignments and a game** (`assignments/`, `game/`): five assignments, each with interactive demos, and five game versions that apply them one by one.
3. **A ready-made raylib + VS Code project template**: clone it, delete a couple of folders, and you have a working C++/raylib setup in minutes.

> Coming from a search for "raylib C++ template"? Skip to **[TEMPLATE.md](./TEMPLATE.md)** for the clean-slate setup guide.

---

## Algorithms Implemented

| Assignment | Algorithm | Library files (`algorithms/`) |
|---|---|---|
| 1 | ✅ DDA line | `dda.cpp` / `.h` |
| 1 | ✅ Bresenham line (any slope) | `bresenham_line.cpp` / `.h` |
| 2 | ✅ Bresenham circle | `bresenham_circle.cpp` / `.h` |
| 2 | ✅ Midpoint circle | `midpoint_circle.cpp` / `.h` |
| 3 | ✅ Translate, scale, rotate (about any pivot) | `transform2d.cpp` / `.h` |
| 3 | ✅ Composite transformations (3×3 matrices) | `matrix3.cpp` / `.h` |
| 4 | ✅ Flood fill (4- and 8-connected) | `fill.cpp` / `.h` |
| 4 | ✅ Boundary fill (4- and 8-connected) | `fill.cpp` / `.h` |
| 4 | ✅ Scan-line polygon fill | `fill.cpp` / `.h` |
| 5 | ✅ Cohen-Sutherland line clipping | `clipping.cpp` / `.h` |
| 5 | ✅ Liang-Barsky line clipping | `clipping.cpp` / `.h` |
| support | ✅ Pixel canvas (lets fills read pixels back) | `canvas.cpp` / `.h` |

**Why a canvas?** Fill algorithms must *read* pixels, and raylib's `DrawPixel` can only write. `canvas` gives every algorithm one function, `plot(x, y, color)`. By default it calls `DrawPixel`; after `canvasEnable(w, h)` it writes into a software pixel buffer that the fills can read, and `canvasPresent()` shows that buffer on screen.

---

## Requirements

You need three things before this will compile: **a C++ compiler (MinGW-w64)**, **the raylib library**, and **VS Code** (optional, but this repo is pre-configured for it).

| Tool | Link | Notes |
|---|---|---|
| MinGW-w64 (g++, gdb) | [github.com/niXman/mingw-builds-binaries](https://github.com/niXman/mingw-builds-binaries/releases) | Grab a release with `posix` threads + `seh` exceptions for x86_64. Extract, then add its `bin/` folder to your PATH (this repo assumes `C:/mingw64/bin`). |
| raylib (prebuilt Windows binaries) | [github.com/raysan5/raylib/releases](https://github.com/raysan5/raylib/releases) | Download the `raylib-x.x_win64_mingw-w64.zip` for your raylib version. |
| raylib official site | [raylib.com](https://www.raylib.com/) | Docs, cheatsheet, examples. |
| VS Code | [code.visualstudio.com](https://code.visualstudio.com/) | Install the **C/C++** extension (ms-vscode.cpptools). |

**From the raylib zip**, you only need these files, copied into this repo:

- `include/` → `raylib.h`, `raymath.h`, `rlgl.h`
- `lib/` → `libraylib.a`, `libraylibdll.a`, `raylib.dll`

This repo already ships with them in place, so if you use it as a template you can skip this step.

---

## Project Structure

```text
CG_raylib/
│   main.cpp                  # sandbox
│   README.md
│   TEMPLATE.md
│   .gitignore
│
├───.vscode/                  # tasks.json, launch.json, c_cpp_properties.json
├───scripts/
│       run.ps1               # builds + runs one .cpp file (used by tasks.json)
│
├───algorithms/               # the library: header + source per algorithm
│       canvas, dda, bresenham_line, bresenham_circle, midpoint_circle,
│       transform2d, matrix3, fill, clipping        (each .cpp + .h)
│
├───assignments/
│   ├───assignment1_lines/            assignment1_dda, assignment1_bresenham
│   ├───assignment2_circles/          assignment2_bresenhamcircle, assignment2_midpointCircle
│   ├───assignment3_transformations/  translate, scale, rotate, sequential, composite
│   ├───assignment4_fill/             floodfill, boundaryfill, scanline
│   └───assignment5_clipping/         cohen_sutherland, liang_barsky
│
├───game/                     # the Mario Kart scene, one version per assignment
│   │   gameversion0.1.cpp ... gameversion0.5.cpp
│   └───alt/                  # alternative drafts (gamealt0.1, gamealt0.2)
│
├───bin/                      # compiled .exe files (git-ignored)
├───docs/screenshots/         # screenshots of each version
├───include/                  # raylib.h, raymath.h, rlgl.h
└───lib/                      # libraylib.a, libraylibdll.a, raylib.dll
```

**Two kinds of files.** The early files (`assignment1_*`, `assignment2_*`, `gameversion0.1`, `gameversion0.2`, `game/alt/`) are frozen, self-contained snapshots: each one contains its own copy of the algorithms because coursework submissions have to stand alone. Everything from Assignment 3 on (`assignment3_*` to `assignment5_*`, `gameversion0.3` to `gameversion0.5`) uses the library through `#include "dda.h"` and friends. Treat `algorithms/` as the maintained code and the early files as read-only history.

---

## The Game: one version per assignment

| Version | Assignment | What it adds | Controls |
|---|---|---|---|
| `gameversion0.1` | 1: lines | Mario Kart scene drawn with DDA lines | none |
| `gameversion0.2` | 2: circles | Wheels and coins with Bresenham circles | none |
| `gameversion0.3` | 3: transformations | Car goes through scale → rotate → translate; spinning wheels, bobbing coins | Arrows move, `Q`/`E` rotate, `Z`/`X` scale, `R` reset |
| `gameversion0.4` | 4: fills | Scan-line (sky, grass, gate), flood fill (road, coins), boundary fill (car, wheels, arch) | `Space` toggles fills on/off |
| `gameversion0.5` | 5: clipping | Playable: steer, collect coins, dodge cones; kerbs, lane dashes and sparks are clipped | `Left`/`Right` steer, `Up`/`Down` speed, `C` clip mode, `[` `]` resize clip window, `Enter` restart |

---

## Building & Running

### The easy way (VS Code)

1. Open the `CG_raylib` folder in VS Code.
2. Open any `.cpp` file (a game version, an assignment demo, `main.cpp`).
3. Press **`Ctrl+Shift+B`**.

That builds the open file into `bin/` and runs it. If the file `#include`s any header from `algorithms/`, the whole library is linked in automatically; otherwise the file is compiled on its own. To build without running, use **Terminal → Run Task → Build Current File**.

### From a terminal

Run from the project root, `C:\dev\CPP\CG_raylib`:

```powershell
powershell -File scripts\run.ps1 game\gameversion0.5.cpp
powershell -File scripts\run.ps1 assignments\assignment4_fill\assignment4_floodfill.cpp
powershell -File scripts\run.ps1 main.cpp -BuildOnly      # build only, do not run
```

The executable lands in `bin\`, for example `.\bin\gameversion0.5.exe`.

### Adding your own file

Create it anywhere in the project, include what you need (`#include "dda.h"`, `#include "fill.h"`, ...), and press `Ctrl+Shift+B`. No `tasks.json` edit is needed.

---

## The Demo Programs

### Assignment 3: transformations

| Program | Shows | Controls |
|---|---|---|
| `assignment3_translate` | `x' = x + tx` | Arrows, `R` |
| `assignment3_scale` | Scaling about the origin vs the object's centre | Left/Right `sx`, Up/Down `sy`, `U` uniform, `F` fixed point, `R` |
| `assignment3_rotate` | Rotating about the origin, the object's centre or the mouse | `Q`/`E`, `P` pivot, `R` |
| `assignment3_sequential` | Scale → Rotate → Translate step by step; order matters | `Space` next step, `O` swap order, `Q`/`E`, `Z`/`X`, arrows |
| `assignment3_composite` | One 3×3 matrix gives the same result as the step-by-step version | `Q`/`E`, `Z`/`X`, arrows, `R` |

### Assignment 4: fills

| Program | Shows | Controls |
|---|---|---|
| `assignment4_floodfill` | Fills by colour; 8-connected fill leaks through a diagonal outline | Click, `C` colour, `T` 4/8-connected, `R` |
| `assignment4_boundaryfill` | Fills up to a boundary colour; a two-colour outline leaks | Click, `C`, `B` boundary colour, `T`, `R` |
| `assignment4_scanline` | A sweep line, edge crossings and pairwise filling | `1`/`2`/`3` presets, `4` draw your own, `Space` replay, Up/Down speed |

### Assignment 5: line clipping

| Program | Shows | Controls |
|---|---|---|
| `assignment5_cohen_sutherland` | Region codes, trivial accept/reject, edge intersections | Mouse moves a line, click sets the anchor, arrows move the window, `[` `]` resize, `R` |
| `assignment5_liang_barsky` | The p, q and q/p table plus t0 and t1 | Same controls |

---

## Troubleshooting

| Problem | Fix |
|---|---|
| **"No build task to run found"** | `.vscode/tasks.json` is missing or has a typo. It must define a task with `"group": { "kind": "build", "isDefault": true }` that calls `scripts/run.ps1`. Check the JSON is valid, then run **Developer: Reload Window**. |
| **Red squiggle on `#include "dda.h"`, but it compiles** | IntelliSense doesn't know about `algorithms/`. In `.vscode/c_cpp_properties.json` set `includePath` to `["${workspaceFolder}/include", "${workspaceFolder}/algorithms"]`. Do not change the `#include` lines. |
| **`cannot find -lraylib`** | You are not building from the project root. `cd` to the project folder, or use `Ctrl+Shift+B`, which does it for you. |
| **`multiple definition of ...`** | An old copy of an algorithm is still in `algorithms/` (for example `fill_algorithms.cpp` or `line_clipping.cpp`). Delete files that are not in the table above. |
| **`g++` is not recognized** | MinGW is not on your PATH. The script uses the full path `C:/mingw64/bin/g++.exe`; edit it in `scripts/run.ps1` if your MinGW lives elsewhere. |
| **`.exe` cannot be written** | The game is still running. Close its window, then rebuild. |
| **Strange compile errors about `#the` or `Remove-Item`** | Terminal commands were pasted into a source file instead of the terminal. Click inside the terminal panel before pasting, and restore the file from git. |
| **A fill covers the whole screen** | The outline has a gap, or you used 8-connected fill on a diagonal outline. Use the 4-connected versions, or pass `maxPixels` to `boundaryFill4` so a leaking fill is undone. |

---

## Goal

To build a small, reusable C++ graphics-algorithms library while gaining a genuine, from-first-principles understanding of the classic Computer Graphics algorithms, and to give anyone starting a raylib + C++ + VS Code project on Windows a clean, working setup to fork from.