3D Asset Viewer
===============

Features
--------

* Wavefront OBJ format
* Scene
* Scrolling animation
* Rotation animation
* Windows [1], Linux [2], Mac [3] support
* Dependencies: OpenGL, GLUT, Math

Setup
-----

For Windows use `winget`:

```
winget install cmake Git.Git
```

Install manually <a href="https://winlibs.com">winlibs.com</a> binary package of MinGW-64,
Install <a href="https://www.msys2.org">MSYS2</a>, build and intall freeglut from sources.

```
cmake -G "MinGW" -S . B . -DCMAKE_INSTALL_PREFIX=E:\Software\Systems\mingw64\
make install
```

For Ubuntu use `apt`:

```
sudo apt install freeglut3-dev mesa-utils
```

For Mac use `brew`:

```
brew install freeglut
```

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
gcc -o 3dav 3dav.c -framework OpenGL -framework GLUT \
                   -I /opt/homebrew/Cellar/freeglut/3.6.0/include
```

Credits
-------

* Namdak Tonpa
