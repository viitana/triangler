# triangler

A rudimentary 3D graphics engine and model viewer. Built from scratch as a personal excercise in modern 3D graphics pipelines and APIs. Uses an OpenGL 3.3 -based renderer.

![Screenshot](/misc/triangler.png?raw=true "The Stanford bunny visualized in triangler")

### Main features
- OpenGL renderer with TrueType text, 3D object and line shaders
- Simple diffuse shading for objects
- Simple OBJ loading (vertices, faces)
- Automatic surface normal generation

### Prerequisites
##### Environment
Microsoft Visual Studio is highly recommended for easy setup. Tested on VS Community 2019.
##### Dependencies
- [glew](http://glew.sourceforge.net)
- [glm](https://glm.g-truc.net)
- [glfw](https://glfw.org)
- [freetype](https://freetype.org)
- [boost](https://boost.org)

[vcpkg](https://github.com/microsoft/vcpkg) is *highly* recommended for installing the libs hassle-free. If using it, make sure to also install the `opengl` library.

### Usage
The source repository is a Microsoft Visual Studio solution. Assuming you have Visual Studio:
- Clone repo
- Open [triangler.sln](/triangler.sln)

### License
See [LICENSE](/LICENSE).
