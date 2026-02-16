@echo off
rmdir /s /q buildFx
mkdir buildFx
cd buildFx
cmake -S .. -DBUILD_AS_FX=ON -DPROJECT_NAME="2686VFX" -G "Visual Studio 18 2026"
echo.
echo ==========================================
echo  FX Project Generated in /buildFx
echo ==========================================
