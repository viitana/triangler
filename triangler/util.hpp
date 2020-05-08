#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Object3D.hpp"

#define PI 3.1415927f

void genNormals(Mesh& mesh);
void genRandomColors(Mesh& mesh);
void genIcosahedron(Mesh& mesh);
void genIcosphere(Mesh& mesh, int subdivisions);
void genIcosahedron(Mesh& mesh);
void LoadOBJf(Object3D* mesh, std::string path, glm::vec3 offset = glm::vec3(0));
void loadIcosphere(Object3D* obj, int subdivisions);
void genRing(Object3D* obj, int points, glm::vec4 color);
void genGrid(Mesh& mesh, const float dim, const int segments, const int subgrids, const glm::vec4 color);
void printVec(glm::vec3 v);
void addDebugVector(Mesh& mesh, const glm::vec3 start, const glm::vec3 end, const glm::vec4 color);
const glm::vec3 ClosestCirlePoint(const glm::vec3 mid, const float r, const glm::vec3 p);
