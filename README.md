Wolves Among the Sheep
======================

Poisoning security tools that use obsolate crypto.

Works-on-my-machine version, feedback is welcome!

Dependencies
------------

* Visual Studio 2012 to compile the projects (Express will do)
* PHP (CLI) >=5.4 to run [longEgg](https://github.com/natmchugh/longEgg) (referenced as submodule)
* [Fastcoll](https://www.win.tue.nl/hashclash/) for collisions

Usage
-----

Extract `fastcoll` to the `fastcoll` directory. Name the executable fastcoll.exe
Put php.exe in your PATH.

Use `shepherd.bat` to generate wolf.exe and sheep.exe:

```
> shepherd.bat YOURPASSWORD your_shellcode.raw
```

After this step you should have your two colliding binaries (`sheep.exe` and `wolf.exe` in the `long\_egg` directory).

For more information see the [tutorial of Nate McHugh](http://natmchugh.blogspot.co.uk/2015/05/how-to-make-two-binaries-with-same-md5.html), older revisions of this document or the source code...

References
----------

* [Poisonous MD5 - Wolves Among the Sheep](http://blog.silentsignal.eu/2015/05/29/poisonous-md5-wolves-among-the-sheep/)
* [How to make two binaries with same MD5](http://natmchugh.blogspot.co.uk/2015/05/how-to-make-two-binaries-with-same-md5.html)
* [Stop using MD5 now!](http://jumpespjump.blogspot.hu/2014/03/stop-using-md-5-now.html)

Tips and Tricks
---------------

`MD5.php` of longEgg uses PHP 5.4 syntax for declaring arrays. If you're using an older version you should replace `[x, y, z]` declarations to `array(x, y, z)`.

TODO
----

* Improve longEgg to accept parameters

LICENSE
-------

Creative Commons Attribution-NonCommercial 4.0 International (https://creativecommons.org/licenses/by-nc/4.0/)
