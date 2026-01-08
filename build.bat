@echo off
if not exist bin mkdir bin
g++ -std=c++20 -O2 src\*.cpp -Iinclude -o bin\raytraced-frontal-area.exe
if %ERRORLEVEL% equ 0 echo Build succeeded && exit /b 0
echo Build failed
exit /b 1
