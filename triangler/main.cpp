#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "shader.hpp"
#include "app.hpp"

int main(void)
{
	App triangler;
	if (!triangler.Initialize())
		return 1;
	triangler.Run();

	return 0;
}
