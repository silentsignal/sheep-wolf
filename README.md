Wolves Among the Sheep
======================

Poisoning security tools that use obsolate crypto.

Dependencies
------------

* Visual Studio 2012 to compile the projects
* PHP (CLI) to run [longEgg](https://github.com/natmchugh/longEgg) (referenced as submodule)
* [Fastcoll](https://www.win.tue.nl/hashclash/) for collisions

Usage
-----

Set your encryption key in `sheep.cpp`.

Use `shepherd.bat` to generate the appropriate header file with the encrypted shellcode:

```
> shepherd.bat YOURPASSWORD your_shellcode.raw
```

Copy `fastcoll` nin the `long\_egg` directory (for convinience).

Run `long\_egg.php` in its own directory - this way it will find `sheep.exe` copyed next to it by the batch file (with the name `demo` that is currently hardwired in longEgg).

Feed the resulting hash to `fastcoll`:

```
fastcoll -i <state hash from longEgg> -o a b
```

This will create two files `a` and `b` that will be consumed by `long\_egg.php` when it's executed again.

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
