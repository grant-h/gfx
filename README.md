# Graphics Demos
A collection of random code to create cool graphics.

## Building
* GCC 5.0.0 or greater (C++11 support)
* 64-bit ONLY

Choose a demo, `cd` to the folder, make the cmake build dir, run cmake, and run make.
Example:

```
cd music_test/
mkdir build
cd build
cmake ..
[lots of cmake output]
make
[lots of make output]
cd ..
./music_test
```

### macOS
* The system's default OpenGL headers
* GCC

### Linux
Dependencies: `sudo apt install cmake xorg-dev libglu1-mesa-dev freeglut3-dev`

