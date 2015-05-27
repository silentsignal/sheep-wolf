@echo on

Release\shepherd.exe %1 %2 > sheep\sc.h
msbuild /p:Configuration=Release /t:Build sheep\sheep.vcxproj

copy sheep\Release\sheep.exe long_egg\demo