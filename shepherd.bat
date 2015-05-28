@echo on

Release\shepherd.exe %1 %2 > sheep\sc.h
msbuild /p:Configuration=Release /t:Build sheep\sheep.vcxproj

del long_egg\demo
del long_egg\a
del long_egg\b

copy sheep\Release\sheep.exe long_egg\demo

