@echo on

Release\shepherd.exe %1 %2 > sheep\sc.h
msbuild /p:Configuration=Release /t:Build sheep\sheep.vcxproj

del long_egg\demo
del long_egg\a
del long_egg\b
del long_egg\wolf.exe
del long_egg\sheep.exe

copy sheep\Release\sheep.exe long_egg\demo

cd long_egg
for /f "tokens=1,2,3,4,5,6,7,8" %%g in ('php long_egg.php') do IF %%g == Chainring SET out=%%n
..\fastcoll\fastcoll.exe -i %out% -o a b
php long_egg.php

move devil wolf.exe
move angel sheep.exe
