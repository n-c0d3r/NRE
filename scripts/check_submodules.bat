@echo off

set SOURCE_DIR=%0\..\..\



cd %SOURCE_DIR%
git submodule update --init
git submodule update