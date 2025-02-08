3D Asset Viewer
===============

Features
--------

* Wavefront OBJ format
* Scrolling animation (Swipe Left/Right)
* Rotation animation (Gran Turismo Garage)
* Windows, Linux, Mac support
* Dependencies: OpenGL, GLUT, Math

Setup
-----

For Windows use `winget`:

```
winget install cmake Git.Git
```

Install manually <a href="https://winlibs.com">winlibs.com</a> binary
package of <a href="https://www.mingw-w64.org">MinGW-64</a>,
Install <a href="https://www.msys2.org">MSYS2</a>, build and install
<a href="https://github.com/freeglut/freeglut">freeglut</a> library from sources.

```
cmake -G "MinGW" -S . B . -DCMAKE_INSTALL_PREFIX=E:\Software\Systems\mingw64\
make install
```

For Ubuntu use `apt`:

```
sudo apt install freeglut3-dev mesa-utils build-essential
```

For Mac do nothing.

Compilation
-----------

Windows:

```
gcc -o 3dav 3dav.c -lopengl32 -lglu32 -lfreeglut
```

Linux:

```
gcc -o 3dav 3dav.c -lglut -lGLU -lGL -lm
```

Mac:

```
gcc -o 3dav 3dav.c -framework OpenGL -framework GLUT
```

Credits
-------

* Namdak Tonpa, <a href="https://github.com/BitEdits/">BitEdits Corporation.</a>
