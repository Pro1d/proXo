# proXo
A 3D graphic engine written in C++ from scratch. No hardware acceleration (only the CPU is used).
Two methods are implemented:
 * A real time engine: similar to those for video games but simplified.
 * A ray tracer: longer rendering but with realistic light effects (shadow, reflection, refraction...)

## Sample demo
It reads `.scene` files (*TODO: how to write a scene description file*).
It loads `.ply` files ([Stanford PLY - Wikipedia](https://en.wikipedia.org/wiki/PLY_(file_format))) exported from [Blender](https://www.blender.org/).

## Requirement
* For the sample, the only external libraries needed are [SDL 1.2](https://www.libsdl.org/index.php) and [SDL\_Image](https://www.libsdl.org/projects/SDL_image/). They can be installed on Ubuntu with: `sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev`

## Coming soon
 * *Textures (diffuse, specular, normal, ...) for the ray tracer*
 * *Documentation for scene file format*
 * *Better refraction support (nested material)*
 * *Implement sky box* 
 * *Improve KD-Tree for ray tracer*
 * *Sample:*
  * *Help for keyboard control*
  * *Add few scenes and 3d models in this repository*
  * *Add screenshots*
 * Complete this ToDo list...
