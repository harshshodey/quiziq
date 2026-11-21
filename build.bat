@echo off
setlocal
cd /d "%~dp0"
where g++ >nul 2>nul
if errorlevel 1 (
  if exist C:\msys64\ucrt64\bin\g++.exe (
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
  ) else (
    echo Please install a MinGW-w64 C++17 compiler and add g++ to PATH.
    exit /b 1
  )
)
g++ -std=c++17 -O2 -Wall -Wextra -Werror -municode -mwindows src\main.cpp -o QuizIQ.exe -static -static-libgcc -static-libstdc++ -lgdiplus -lshell32 -lole32 -luser32 -lgdi32 -luuid -lcomdlg32
if errorlevel 1 exit /b 1
g++ -std=c++17 -O2 tests\engine_tests.cpp -o engine_tests.exe -static
if errorlevel 1 exit /b 1
engine_tests.exe
if errorlevel 1 exit /b 1
g++ -std=c++17 -O2 -Wall -Wextra -Werror tests\view_tests.cpp -o view_tests.exe -static -lgdiplus -lshell32 -lole32 -luser32 -lgdi32 -luuid -lcomdlg32
if errorlevel 1 exit /b 1
view_tests.exe
if errorlevel 1 exit /b 1
g++ -std=c++17 -O2 -Wall -Wextra -Werror tests\literary_tests.cpp -o literary_tests.exe -static
if errorlevel 1 exit /b 1
literary_tests.exe
if errorlevel 1 exit /b 1
echo Build and tests successful. Open QuizIQ.exe to play.
