3D Asset Viewer
===============

Features
--------

* Formats: Wavefront OBJ/FBX file formats
* Animations: Rotation, Scrolling, Keyframes
* Crossplatform: Windows, Linux, Mac
* Lightweight: OpenGL, GLU, GLUT/GLFW

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

* Namdak Tonpa, <a href="https://github.com/BitEdits/">BitEdits</a> Corporation.
