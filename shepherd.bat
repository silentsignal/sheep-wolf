@echo on

del sheep\Release\sheep.pch

Release\shepherd.exe %1 %2 > sheep\sc.h
msbuild /p:Configuration=Release /t:Build sheep\sheep.vcxproj

del evilize\a
del evilize\b
del evilize\wolf.exe
del evilize\sheep.exe

cd evilize 

evilize.exe ..\sheep\Release\sheep.exe 

