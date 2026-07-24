# Using This Repo as a Raylib Project Template

This repo doubles as a starter template for any new raylib + C++ + VS Code project on Windows. Follow this if you just want the boilerplate — not the algorithms or assignments.

## 1. Clone / use as GitHub template

```powershell
git clone https://github.com/p-rth12999/cpp-raylib-algorithms.git my-new-project
cd my-new-project
```


Or click **"Use this template"** on GitHub if it's enabled on the repo.

## 2. Remove what you don't need

```powershell
Remove-Item -Recurse -Force ".\algorithms"
Remove-Item -Recurse -Force ".\assignments"
Remove-Item -Recurse -Force ".\game"
Remove-Item -Recurse -Force ".\outputs"
Remove-Item -Force ".\main.exe"
```

Keep: `include/`, `lib/`, `.vscode/`, `main.cpp`, `.gitignore`.

## 3. Reset `main.cpp` to a blank raylib window

```cpp
#include "raylib.h"

int main()
{
    InitWindow(800, 600, "My Raylib Project");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Hello, raylib!", 190, 200, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

## 4. Simplify the build task

Open `.vscode/tasks.json` and reduce it to a single default task:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build",
            "type": "shell",
            "command": "C:/mingw64/bin/g++.exe",
            "args": [
                "main.cpp",
                "-o", "main.exe",
                "-I", "include",
                "-L", "lib",
                "-lraylib", "-lopengl32", "-lgdi32", "-lwinmm"
            ],
            "group": { "kind": "build", "isDefault": true },
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

## 5. Check your MinGW path

`tasks.json` and `.vscode/c_cpp_properties.json` both hardcode `C:/mingw64/bin/`. If your MinGW-w64 install lives somewhere else, update the `compilerPath` / `command` fields in both files to match.

## 6. Build & run