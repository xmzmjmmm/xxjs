@echo off
title XXJS BUILD TERMINAL
color 0a

echo -------------------------------------------
powershell -c "write-host ('['+[char]0x8fdb+[char]0x5ea6+'] '+[char]0x6b63+[char]0x5728+[char]0x6784+[char]0x5efa+[char]0x661f+[char]0x661f+[char]0x52a0+[char]0x901f) -f Green"
echo -------------------------------------------
powershell -c "write-host ('['+[char]0x4fe1+[char]0x606f+'] '+[char]0x6e90+[char]0x7801+[char]0x6587+[char]0x4ef6+': xxjs.cpp') -f Green"
echo -------------------------------------------

echo [INFO] Compiling...
g++ -O3 -std=c++17 xxjs.cpp -o xxjs.exe -lwininet -lshlwapi -lshell32 -lole32 -lgdiplus -ladvapi32 -mwindows -municode

if %errorlevel% equ 0 (
    echo.
    powershell -c "write-host ('['+[char]0x6210+[char]0x529f+'] '+[char]0x7f16+[char]0x8bd1+[char]0x4efb+[char]0x52a1+[char]0x5df2+[char]0x5b8c+[char]0x6210+'!') -f Cyan"
    powershell -c "write-host ('['+[char]0x8f93+[char]0x51fa+'] '+[char]0x751f+[char]0x6210+[char]0x6587+[char]0x4ef6+': xxjs.exe') -f Cyan"
    echo -------------------------------------------
    powershell -c "write-host ([char]0x8bf7+[char]0x76f4+[char]0x63a5+[char]0x8f93+[char]0x5165+' xxjs.exe '+[char]0x8fd0+[char]0x884c+[char]0x7a0b+[char]0x5e8f) -f Green"
) else (
    echo.
    powershell -c "write-host ('['+[char]0x5931+[char]0x8d25+'] '+[char]0x7f16+[char]0x8bd1+[char]0x51fa+[char]0x9519) -f Red"
)

echo.
cmd /k
