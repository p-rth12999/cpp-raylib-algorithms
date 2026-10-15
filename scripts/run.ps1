# Builds and runs ONE .cpp file with raylib.
# If the file #includes any header from algorithms/, every algorithms/*.cpp
# is linked in automatically. Files that define their own functions
# (the early assignments / game versions) are compiled on their own.
#
#   powershell -File scripts/run.ps1 <file.cpp> [-BuildOnly]
param(
    [Parameter(Mandatory = $true)][string]$File,
    [switch]$BuildOnly
)

Set-Location (Split-Path -Parent $PSScriptRoot)      # project root

$name    = [System.IO.Path]::GetFileNameWithoutExtension($File)
$sources = @($File)

$headers = @(Get-ChildItem "algorithms" -Filter *.h | ForEach-Object { $_.BaseName })
if ($headers.Count -gt 0) {
    $pattern = '#include\s+"(?:[\w./]*/)?(' + ($headers -join '|') + ')\.h"'
    if (Select-String -Path $File -Pattern $pattern -Quiet) {
        $sources += @(Get-ChildItem "algorithms" -Filter *.cpp | ForEach-Object { $_.FullName })
    }
}

New-Item -ItemType Directory -Force "bin" | Out-Null
$exe = "bin\$name.exe"

& C:/mingw64/bin/g++.exe -std=c++17 @sources -o $exe -I include -I algorithms -L lib -lraylib -lopengl32 -lgdi32 -lwinmm
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (-not $BuildOnly) { & ".\$exe" }
