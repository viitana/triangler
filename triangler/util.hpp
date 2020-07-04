#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "mesh.hpp"

#define PI 3.1415927f

void printVec(glm::vec3 v);
void addDebugVector(Mesh& mesh, const glm::vec3 start, const glm::vec3 end, const glm::vec4 color);
const glm::vec3 ClosestCirlePoint(const glm::vec3 mid, const float r, const glm::vec3 p);
