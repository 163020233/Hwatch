@echo off
@REM work dir is ../../../
set isOnlyRename=%1
set "filename_simulator=CMakeLists.txt.simulator"
set "filename_sdk=CMakeLists.txt.sdk"
set "dirname_res=application\wearable\res"
setlocal enabledelayedexpansion

if not exist %dirname_res% (    
    xcopy /s /i /e tools\simulator\res application\wearable\res
)
if not exist %filename_simulator% (
    if not exist %filename_sdk% (
        copy tools\simulator\CMakeLists.txt.simulator .\
    )
)
if exist %filename_simulator% (
    rename CMakeLists.txt CMakeLists.txt.sdk
    rename CMakeLists.txt.simulator CMakeLists.txt
)
if "%isOnlyRename%" == "YES" (
    echo "-----only rename------"
) else (
echo "======current========="
dir
echo "======end========="
cmake . -G "MinGW Makefiles" -B simulator_build -DBUILD_SIMULATOR=ON -DBUILD_SIMULATOR_BY_UIKIT=ON -DUSE_BACKGROUNDIMAGE=ON -DCMAKE_BUILD_TYPE=Debug -DRELEASE_VERSION=ON
if !errorlevel! neq 0 (
    exit /b 1
)
cd simulator_build
mingw32-make 2> error.txt -j8
if !errorlevel! neq 0 (
    exit /b 1
)
)