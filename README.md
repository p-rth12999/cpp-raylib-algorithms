# C++ Raylib Graphics Algorithms

A from-scratch collection of classic Computer Graphics algorithms — implemented in C++ with **raylib** — plus a ready-to-clone project template for starting your own raylib projects on Windows with MinGW.

Every algorithm here computes pixels manually and plots them with `DrawPixel()`, instead of relying on raylib's built-in shape functions (`DrawLine`, `DrawCircle`, etc.). The goal is to actually understand the algorithm, not just call a library function that does it for you.

---

## What's in this repo

This repo does two jobs at once:

1. **A small graphics-algorithms library** (`algorithms/`) — reusable, header-based, drop into any raylib project.
2. **A ready-made raylib + VS Code project template** — clone it, delete a couple of folders, and you have a working C++/raylib dev setup in minutes.

> Coming from a search for "raylib C++ template"? Skip to **[TEMPLATE.md](./TEMPLATE.md)** for the clean-slate setup guide.

---

## Algorithms Implemented

| Algorithm | Files |
|---|---|
| ✅ DDA Line Algorithm | `algorithms/dda.cpp` / `.h` |
| ✅ Bresenham's Line Algorithm | `algorithms/bresenham_line.cpp` / `.h` |
| ✅ Bresenham's Circle Algorithm | `algorithms/bresenham_circle.cpp` / `.h` |

More to be added over time.

---

## Requirements

You need three things before this will compile: **a C++ compiler (MinGW-w64)**, **the raylib library**, and **VS Code** (optional but this repo is pre-configured for it).

| Tool | Link | Notes |
|---|---|---|
| MinGW-w64 (g++, gdb) | [github.com/niXman/mingw-builds-binaries](https://github.com/niXman/mingw-builds-binaries/releases) | Grab a release with `posix` threads + `seh` exceptions for x86_64. Extract, then add its `bin/` folder to your PATH (this repo assumes `C:/mingw64/bin`). |
| raylib (prebuilt Windows binaries) | [github.com/raysan5/raylib/releases](https://github.com/raysan5/raylib/releases) | Download the `raylib-x.x_win64_mingw-w64.zip` for your raylib version. |
| raylib official site | [raylib.com](https://www.raylib.com/) | Docs, cheatsheet, examples. |
| VS Code | [code.visualstudio.com](https://code.visualstudio.com/) | Install the **C/C++** extension (ms-vscode.cpptools). |

**From the raylib zip**, you only need three folders/files, copied into this repo:
- `include/` → `raylib.h`, `raymath.h`, `rlgl.h`
- `lib/` → `libraylib.a`, `libraylibdll.a`, `raylib.dll`

(This repo already ships with these placed correctly — see structure below — so if you're using this repo *as a template*, you likely don't need to redo this step. If `raylib.h` isn't found by IntelliSense/the compiler, it's because these files are missing or your MinGW path differs — see [TEMPLATE.md](./TEMPLATE.md).)

---

## Project Structure

```text
CG_raylib/
│   main.cpp                 # sandbox — currently demos bresenham_circle
│   README.md
│   .gitignore
│
├───algorithms/               # reusable library — header + source per algorithm
│       dda.cpp / dda.h
│       bresenham_line.cpp / bresenham_line.h
│       bresenham_circle.cpp / bresenham_circle.h
│
├───assignments/              # course assignment submissions (self-contained,
│                              # no headers used — see note below)
│       assignment1_dda.cpp
│       assignment1_bresenham.cpp
│       assignment2_bresenhamcircle.cpp
│       gameversion0.1.cpp
│       gameversion0.2.cpp
│
├───game/                     # the actual long-term project (WIP)
│
├───outputs/                  # screenshots / recordings of each version
│       ├───assignments/
│       └───game/
│
├───include/                  # raylib.h, raymath.h, rlgl.h
├───lib/                      # libraylib.a, libraylibdll.a, raylib.dll
└───.vscode/                  # build + debug tasks, already configured
```

**Why `assignments/` doesn't use headers:** those files are frozen, self-contained snapshots submitted for coursework — each one includes the full function body inline rather than `#include`-ing from `algorithms/`, since submissions need to stand alone. `algorithms/` is the actual maintained library; `assignments/` is a point-in-time export from it. Don't expect these two to always match line-for-line — treat `assignments/` as read-only history.

---

## Building & Running

This repo ships with VS Code tasks pre-configured (`.vscode/tasks.json`) — one per buildable target.

1. Open the folder in VS Code.
2. `Ctrl+Shift+P` → **Tasks: Run Task** → pick the target (e.g. `Build Main`, `Build Game Version 0.2`).
3. Run the resulting `.exe` from the integrated terminal:
```powershell
   .\main.exe
   # or
   .\assignments\gameversion0.2.exe
```

`Ctrl+Shift+B` runs whichever task is marked `"isDefault": true` in `tasks.json` (currently `Build Main`) — for anything else, use **Tasks: Run Task** and pick it explicitly.

---

## Goal

To build a small, reusable C++ graphics-algorithms library while gaining a genuine, from-first-principles understanding of the classic Computer Graphics algorithms — and to give anyone starting a raylib + C++ + VS Code project on Windows a clean, working setup to fork from.