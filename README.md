# triangler

A rudimentary OpenGL 3.3 graphics engine and model viewer. Built from scratch as a personal excercise in modern 3D graphics pipelines and APIs.

![Screenshot](/misc/triangler.png?raw=true "A couple of Stanford bunnies and other objects visualized in triangler")

### Main features
* Observer-based shader pipeline
  * Bind/load/reload shaders, uniforms & vertex attributes only as needed
* Dynamic TrueType text rendering
* Simple diffuse shading for objects
* Simple OBJ loading (vertices, faces)
* Handy ray-based cursor object & camera controls

### Prerequisites
##### Environment
Microsoft Visual Studio is recommended for easy setup. Tested on VS Community 2019.
##### Dependencies
- [glew](http://glew.sourceforge.net)
- [glm](https://glm.g-truc.net)
- [glfw](https://glfw.org)
- [freetype](https://freetype.org)
- [boost](https://boost.org)

[vcpkg](https://github.com/microsoft/vcpkg) is recommended for installing the libs hassle-free. If using it, make sure to also install the `opengl` library.

### Usage
Assuming you use Visual Studio:
- Clone repo
- Open [triangler.sln](/triangler.sln)

### Changelog
```
*** 0.0.3 ***
- Observer-based combined render & shader pipeline for 3D objects
  - Unified rendering pipeline for tri meshes, lines, points
  - Generate & bind rendering resources in-object
  - Bind/load/reload shaders, uniforms & vertex attribs only as needed
- Selected object axis ring & point indicators
- Toggleable flat shading
*** 0.0.2 ***
- Ray-based cursor-object grab rotation control
- Arrow key -controllable debug menu
- Toggleable wireframe shading
*** 0.0.1 ***
- Basic diffuse shaded render pipeline for 3D objects
- Separate render pipelines for Truetype text, lines and grid
- Ray-based cursor-grid grab camera control
- Simple .obj loader
```

### License
See [LICENSE](/LICENSE).
