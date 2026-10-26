@echo off
setlocal
cd /d "%~dp0"

set "EXE=build\Release\DayDoom.exe"
if exist "build\Release\DayDoom_v002.exe" (
    set "EXE=build\Release\DayDoom_v002.exe"
) else if exist "build\Release\DayDoom_v001.exe" (
    set "EXE=build\Release\DayDoom_v001.exe"
)

if not "%~1"=="" (
    start "" "%EXE%" %*
    goto :eof
)

echo ========================================================
echo   DAYDOOM 2.5D [v002] - Selecione a Resolucao de Execucao
echo ========================================================
echo  [1] Widescreen 16:9 (480x270) - Padrao PC e Mobile Landscape
echo  [2] Doom Classico 13h (320x200) - Retro MS-DOS 1993
echo  [3] Retro CRT 4:3 (320x240) - Proporcao Classica CRT
echo ========================================================
set "opt=1"
set /p opt="Escolha uma opcao [1-3] (Padrao: 1): "

if "%opt%"=="2" (
    echo Iniciando no modo Doom Classico 13h (320x200)...
    start "" "%EXE%" --doom
) else if "%opt%"=="3" (
    echo Iniciando no modo Retro CRT 4:3 (320x240)...
    start "" "%EXE%" --43
) else (
    echo Iniciando no modo Widescreen 16:9 (480x270)...
    start "" "%EXE%"
)

