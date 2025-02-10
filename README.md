3D Asset Viewer
===============

Features
--------

* Formats: Wavefront OBJ/FBX file formats
* Animations: Rotation, Scrolling, Keyframes
* Crossplatform: Windows, Linux, Mac
* Lightweight: OpenGL, GLUT/GLFW

Setup
-----

For Windows use `winget`:

```
winget install cmake Git.Git
```

Install manually <a href="https://winlibs.com">winlibs.com</a> binary
package of <a href="https://www.mingw-w64.org">MinGW-64</a>,
Install <a href="https://www.msys2.org">MSYS2</a>, build and install
<a href="https://github.com/freeglut/freeglut">FreeGLUT</a> or
<a href="https://github.com/glfw/glfw">GLFW</a> libraries from sources.

```
cmake -G "MinGW" -S . B . -DCMAKE_INSTALL_PREFIX=E:\Software\Systems\mingw64\
make install
```

For Ubuntu use `apt`:

```
sudo apt install freeglut3-dev libglfw3-dev mesa-utils build-essential
```

For Mac use `brew`:

```
brew install glfw
```

Compilation
-----------

Windows:

```
gcc -o 3dav 3dav_glfw.c -lopengl32 -lglfw3 -lgdi32
gcc -o 3dav 3dav_glut.c -lopengl32 -lfreeglut
gcc -o 3dav 3dav_sdl.c  -lopengl32 -lSDL2 -lSDL2_ttf
```

Linux:

```
gcc -o 3dav 3dav_glfw.c -lglfw -lGL -lm
gcc -o 3dav 3dav_glut.c -lglut -lGL -lm
```

Mac:

```
gcc -o 3dav 3dav_glut.c -framework OpenGL -framework GLUT
gcc -o 3dav 3dav_glfw.c -framework OpenGL -lglfw
```

Credits
-------

* Namdak Tonpa, <a href="https://github.com/BitEdits/">BitEdits</a> Corporation.
