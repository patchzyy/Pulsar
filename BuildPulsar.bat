@echo off
SETLOCAL EnableDelayedExpansion
cls

:: Clear old build files
echo Cleaning up old build files...
if exist build\*.o (
    del /q build\*.o
    if %ErrorLevel% neq 0 (
        echo Error: Failed to delete old object files in build directory.
        exit /b 1
    )
) else (
    echo No old object files to delete.
)

:: Check for debug flag and configure
SET "debug="
:: Uncomment the next two lines to enable debug settings based on arguments
:: SET "cwDWARF="
:: if "%1" equ "-d" SET "debug=-debug -map=^"Dolphin Emulator\Maps\RMCP01.map^" -readelf=^"C:\MinGW\bin\readelf.exe^"" && SET "cwDWARF=-g"

:: Define paths
SET "ENGINE=.\KamekInclude"
SET "GAMESOURCE=.\GameSource"
SET "PULSAR=.\PulsarEngine"
SET "CC=D:\Source\Common\cw\mwcceppc.exe"
SET "RIIVO=C:\Users\ZachPL\AppData\Roaming\Dolphin Emulator\Load\Riivolution\RetroRewind6"
SET "CFLAGS=-I- -i %ENGINE% -i %GAMESOURCE% -i %PULSAR% -opt all -inline auto -enum int -proc gekko -fp hard -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 %cwDWARF%"

:: Create build directory if it doesn't exist
if not exist "build" mkdir build

:: Ensure the CodeWarrior path exists
if not exist "%CC%" (
    echo Error: CodeWarrior path not found. Please set the correct path for CC.
    exit /b 1
)

:: Collect .cpp files and check if they need rebuilding
SET OBJECTS=
SET NEED_REBUILD=0

:: Always rebuild kamek.cpp as it's critical
echo Compiling kamek.cpp...
%CC% %CFLAGS% -c -o "build/kamek.o" "%ENGINE%\kamek.cpp"
if %ErrorLevel% neq 0 (
    echo Error: Failed to compile kamek.cpp.
    exit /b 1
)
SET "OBJECTS=build/kamek.o"

:: Check and compile each .cpp file only if needed
for /R %PULSAR% %%f in (*.cpp) do (
    SET "NEED_REBUILD=0"
    SET "OBJ_FILE=build\%%~nf.o"
    
    :: Check if .o file exists
    if not exist "!OBJ_FILE!" (
        SET "NEED_REBUILD=1"
    ) else (
        :: Compare timestamps
        for %%A in ("%%f") do set "CPP_TIME=%%~tA"
        for %%B in ("!OBJ_FILE!") do set "OBJ_TIME=%%~tB"
        
        if "!CPP_TIME!" gtr "!OBJ_TIME!" (
            SET "NEED_REBUILD=1"
        )
    )
    
    :: Rebuild if needed
    if !NEED_REBUILD! equ 1 (
        echo Compiling %%f...
        %CC% %CFLAGS% %DEFINE% -c -o "!OBJ_FILE!" "%%f"
        if !ErrorLevel! neq 0 (
            echo Error: Failed to compile %%f.
            exit /b 1
        )
    ) else (
        echo Skipping %%f ^(not modified^)
    )
    
    SET "OBJECTS=!OBJECTS! !OBJ_FILE!"
)

:: Linking
echo Linking... %time%
".\KamekLinker\Kamek.exe" %OBJECTS% %debug% -dynamic -externals="%GAMESOURCE%/symbols.txt" -externals="%GAMESOURCE%/AntiCheat.txt" -versions="%GAMESOURCE%/versions.txt" -output-combined=build\Code.pul
if %ErrorLevel% neq 0 (
    echo Error: Linking failed.
    exit /b 1
)

:: Copy output to Riivolution folder if defined
if defined RIIVO (
    echo Copying binaries to %RIIVO%\Binaries...
    xcopy /Y build\*.pul "%RIIVO%\Binaries" /i /q
    if %ErrorLevel% neq 0 (
        echo Error: Failed to copy binaries to %RIIVO%\Binaries.
        exit /b 1
    ) else (
        echo Binaries copied successfully.
    )
)

echo Build completed successfully.
:end
ENDLOCAL