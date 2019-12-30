#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "mesh.hpp"

void genNormals(Mesh& mesh);
void genRandomColors(Mesh& mesh);
void genIcosahedron(Mesh& mesh);
void genIcosphere(Mesh& mesh, int subdivisions);
void genIcosahedron(Mesh& mesh);
void LoadOBJf(Mesh& mesh, std::string path);
void genGrid(Mesh& mesh, const float dim, const int segments, const int subgrids, const glm::vec4 color);
void printVec(glm::vec3 v);
