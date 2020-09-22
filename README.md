# Graphics Demos
A collection of random code to create cool graphics.

## Development Links
* Frustum Culling - https://www.youtube.com/watch?v=4p-E_31XOPM
* OpenGL 4.4 Scene Rendering Techniques - http://on-demand.gputechconf.com/gtc/2014/presentations/S4379-opengl-44-scene-rendering-techniques.pdf

## Future Ideas
* Light objects
 - Scene has multiple lights associated with it
 - Passed into programs when rendering objects
* Animation controller
 - Driving animations of objects based on timing
* Resource description file
 - Construct shaders from commands (create_texture name path | create_model name path | create_program name shader.vert shader.frag)
 - On-demand loading for fast start up (when not everything is immediately required, e.g multiple scenes)
* Path based camera movement
 - Keyframing of camera movement based on flying/hotkey
 - Bezier curve interpolation for smooth pathing
 - Smooth quaternion transitions for camera orientation as well (allowing automatic roll to simulate turning)
* Director
 - Allow for timeline control of simulation
 - GNU Rocket-like interface with better controls
 - Leverage https://github.com/CedricGuillemet/ImGuizmo for the sequencer view

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
make -j
[lots of make output]
cd ..
./music_test
```

### macOS
* The system's default OpenGL headers
* GCC

### Linux
Dependencies: `sudo apt install cmake xorg-dev libglu1-mesa-dev freeglut3-dev`
