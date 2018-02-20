@echo off
pushd bin
cl /Zi /Fesdl2_testing_win.exe ../*.cpp /I ../include  /link /LIBPATH:../lib/x64 SDL2.lib SDL2main.lib /SUBSYSTEM:CONSOLE
popd
