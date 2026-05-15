cls
del /f /q main_sdl.exe
del /f /q *.o
set LIBRARY=-lmingw32 -lSDL2_ttf -lSDL2
c:\gcc-14.2.0-no-debug\bin\gcc cJSON.c %LIBRARY% -I.\ -L.\ -D_DEBUG_PRINT -c
c:\gcc-14.2.0-no-debug\bin\gcc cJSON_Utils.c %LIBRARY% -I.\ -L.\ -D_DEBUG_PRINT -c
c:\gcc-14.2.0-no-debug\bin\gcc geometry.c %LIBRARY% -I.\ -L.\ -D_DEBUG_PRINT -c
c:\gcc-14.2.0-no-debug\bin\gcc gear_simple.c %LIBRARY% -I.\ -L.\ -D_DEBUG_PRINT -c
c:\gcc-14.2.0-no-debug\bin\gcc gear.c %LIBRARY% -I.\ -L.\ -D_DEBUG_PRINT -c
c:\gcc-14.2.0-no-debug\bin\gcc gear_sdl.c %LIBRARY% -I.\ -L.\ -D_DEBUG_PRINT -c
c:\gcc-14.2.0-no-debug\bin\gcc dxf.c %LIBRARY% -I.\ -L.\ -D_DEBUG_PRINT -c
c:\gcc-14.2.0-no-debug\bin\gcc gear_calculator.c %LIBRARY% -I.\ -L.\ -D_DEBUG_PRINT -c
c:\gcc-14.2.0-no-debug\bin\gcc main_sdl.c %LIBRARY% -I.\ -L.\ -o main_sdl gear_calculator.o dxf.o geometry.o gear.o gear_simple.o gear_sdl.o cJSON.o cJSON_Utils.c
main_sdl.exe