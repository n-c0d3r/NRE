@echo off

set SOURCE_DIR=%0\..\..\
set BUILD_DIR=%SOURCE_DIR%\build\vs2022



if not exist %BUILD_DIR% (
    rem
    mkdir %BUILD_DIR%
)



cmake -G "Visual Studio 17 2022" -S %SOURCE_DIR% -B %BUILD_DIR%