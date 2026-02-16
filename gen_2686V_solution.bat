@echo off
rmdir /s /q build
mkdir build
cd build
cmake -S .. -DBUILD_AS_FX=OFF -DPROJECT_NAME="2686V" -G "Visual Studio 18 2026"
echo.
echo ==========================================
echo  Instrument Project Generated in /build
echo ==========================================
