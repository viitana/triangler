#include <iostream>
#include <sstream>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "app.hpp"
#include "util.hpp"

// shader settings
// lighting
// scene descriptions
// movement modes
// open shader compiler
// grid
// cooler obj load stats
// clean up app members
// res debug test
// paramametrize fnear, ffar (, others?)
// highlight camera focus pos on cam move
// fps independent movement
// consts
// grid duplicates
// funky cam

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	static_cast<App*>(glfwGetWindowUserPointer(window))->HandleCursorMove(xpos, ypos);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	static_cast<App*>(glfwGetWindowUserPointer(window))->HandleResize(width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	static_cast<App*>(glfwGetWindowUserPointer(window))->HandleMouseScroll(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static_cast<App*>(glfwGetWindowUserPointer(window))->HandleKey(key, scancode, action, mods);
}

bool App::Initialize()
{
	if (!InitGLFW())
		return false;

	if (!InitGLEW())
		return false;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_FLAT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	InitRenderMain();
	InitFont();
	InitRenderText();
	InitRenderGrid();

	return true;
}

void App::Run()
{
	do {
		// Framerate timing
		CheckTiming();

		// Keyboard movement
		WASDMove();

		// Main render
		RenderTris();

		// Grid render
		RenderGrid();

		// Debug text render
		RenderDebug();

		// Swap buffers
		glfwSwapBuffers(window_);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwWindowShouldClose(window_) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertex_buffer_);
	glDeleteBuffers(1, &color_buffer_);
	glDeleteBuffers(1, &normal_buffer_);
	glDeleteVertexArrays(1, &vertex_array_id_);
	glDeleteProgram(program_id_);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

void App::CheckTiming() {
	double time = glfwGetTime();
	frametimes_.emplace(glfwGetTime());

	if (frametimes_.size() > 100)
		frametimes_.pop();

	framerate_ = (static_cast<double>(frametime_buffer_size_) - 1.0) / (frametimes_.back() - frametimes_.front());
}

void App::HandleCursorMove(double xpos, double ypos)
{

	glm::vec2 pos(xpos, ypos);

	// Left mouse to rotate object
	if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glm::vec2 delta = cursor_pos_ - pos;
		rotation_[0] -= 0.0015f * delta[1];
		rotation_[1] -= 0.0015f * delta[0];
	}

	// Right mouse to rotate camera
	if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glm::vec2 delta = cursor_pos_ - pos;
		camera_dir_ = glm::rotate(camera_dir_, -0.001f * delta[0], glm::vec3(0, 1, 0));
		glm::vec3 y_rot_axis = glm::cross(camera_dir_, glm::vec3(0, 1, 0));
		camera_dir_ = glm::rotate(camera_dir_, -0.001f * delta[1], y_rot_axis);
	}

	// Middle mouse to move camera
	if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		int xsize, ysize;
		glfwGetWindowSize(window_, &xsize, &ysize);

		float nx = -2.0f * ((xsize - xpos) / xsize) + 1.0f;
		float ny = 2.0f * ((ysize - ypos) / ysize) - 1.0f;

		if (mouse_mid_held_)
		{
			glm::vec3 w = glm::normalize(camera_dir_);
			glm::vec3 u = glm::normalize(glm::cross(w, glm::vec3(0, 1, 0)));
			glm::vec3 v = glm::normalize(glm::cross(u, w));

			float d = 1.f / tanf(glm::radians(45.0f / 2));
			glm::vec3 rd = glm::normalize(nx * u + (16.f / 9.f) * ny * v + d * w);
			float rt = grid_height_ - (glm::dot(glm::vec3(0, 1, 0), camera_pos_)) / glm::dot(glm::vec3(0, 1, 0), rd);

			glm::vec3 heldpos = grid_held_camera_pos_ + rd * rt;

			std::cout << "ray_t: " << rt << std::endl;

			//camera_pos_ += (heldpos - grid_held_camera_pos_);
			//std::cout << heldpos[0] << "  ,  " << heldpos[1] << "  ,  " << heldpos[2] << std::endl;
			//std::cout << "holding: " << camera_pos_[0] << ", " << camera_pos_[1] << ", " << camera_pos_[2] << std::endl;
		}
		else
		{
			std::cout << "new hold" << std::endl;

			glm::vec3 w = glm::normalize(camera_dir_);
			glm::vec3 v = glm::normalize(glm::cross(w, glm::vec3(0, 1, 0)));
			glm::vec3 u = glm::normalize(glm::cross(v, w));

			float d = 1.f / tanf(glm::radians(45.0f / 2));
			glm::vec3 rd = glm::normalize(nx * u + (16.f / 9.f) * ny * v + d * w);
			float rt = grid_height_ - (glm::dot(glm::vec3(0, 1, 0), camera_pos_)) / glm::dot(glm::vec3(0, 1, 0), rd);

			glm::vec3 heldpos = camera_pos_ + rd * rt;

			if (rt < 0)
			{
				grid_held_pos_ = camera_pos_ + rd * rt;
				grid_held_camera_pos_ = camera_pos_;
			}

			mouse_mid_held_ = true;

			//std::cout << heldpos[0] << "  ,  " << heldpos[1] << "  ,  " << heldpos[2] << std::endl;
		}

		//std::cout << "Window size: " << xsize << " x " << ysize << ", mouse pos: " << xpos << " x " << ypos << ", npos:" << nx << " x " << ny << std::endl;
	}
	else
	{
		mouse_mid_held_ = false;
	}

	cursor_pos_ = pos;
}

void App::HandleKey(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window_, GLFW_TRUE);
		if (key == GLFW_KEY_ENTER) {
			glDeleteProgram(program_id_);
			program_id_ = LoadShaders("vertex.shader", "fragment.shader");
			glDeleteProgram(program_id_text_);
			program_id_text_ = LoadShaders("vertex_text.shader", "fragment_text.shader");
			glDeleteProgram(program_id_line_);
			program_id_line_ = LoadShaders("vertex_line.shader", "fragment_line.shader");
		}
	}
}

void App::WASDMove()
{
	if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
		camera_pos_ += glm::normalize(camera_dir_) * camera_speed_;
	if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
		camera_pos_ -= glm::normalize(camera_dir_) * camera_speed_;
	if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
		camera_pos_ += glm::normalize(glm::cross(camera_dir_, glm::vec3(0, 1, 0))) * camera_speed_;
	if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
		camera_pos_ -= glm::normalize(glm::cross(camera_dir_, glm::vec3(0, 1, 0))) * camera_speed_;
}

void App::HandleResize(int width, int height)
{
	glViewport(0, 0, width, height);
	projection_text_ = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
	width_ = width;
	height_ = height;
}

void App::HandleMouseScroll(double xoffset, double yoffset)
{
	std::cout << xoffset << ", " << yoffset << std::endl;
	if (yoffset < 0)
	{
		for (int i = 0; i > yoffset; i--) {
			glm::vec3 delta = camera_dir_ * 0.05f;
			camera_dir_ -= delta;
			camera_pos_ += delta;
		}
	}
	else if (yoffset > 0)
	{
		for (int i = 0; i < yoffset; i++) {
			glm::vec3 delta = camera_dir_ * -0.05f;
			camera_dir_ -= delta;
			camera_pos_ += delta;
		}
	}
}

bool App::InitGLFW()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window_ = glfwCreateWindow(width_, height_, std::string("triangler ").append(TRIANGLER_VERSION).c_str(), NULL, NULL);
	if (window_ == NULL) {
		fprintf(stderr, "Failed to open GLFW window\n");
		getchar();
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window_);

	glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetWindowUserPointer(window_, this);
	glfwSetCursorPosCallback(window_, cursor_position_callback);
	glfwSetWindowSizeCallback(window_, window_size_callback);
	glfwSetScrollCallback(window_, scroll_callback);
	glfwSetKeyCallback(window_, key_callback);

	return true;
}

bool App::InitGLEW()
{
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	return true;
}

void App::InitRenderMain()
{
	// 3D shader program
	program_id_ = LoadShaders("vertex.shader", "fragment.shader");

	// VAO
	glGenVertexArrays(1, &vertex_array_id_);
	glBindVertexArray(vertex_array_id_);

	// Get mesh, setup index & vertice VBO
	//genIcosphere(mesh_, 0);
	LoadOBJf(mesh_, "assets/bunny_lores.obj");
	glGenBuffers(1, &vertex_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
	glBufferData(GL_ARRAY_BUFFER, mesh_.v.size() * sizeof(mesh_.v[0]), mesh_.v.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_.t.size() * sizeof(mesh_.t[0]), mesh_.t.data(), GL_STATIC_DRAW);

	// Get colors, setup VBO
	genRandomColors(mesh_);
	glGenBuffers(1, &color_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
	glBufferData(GL_ARRAY_BUFFER, mesh_.c.size() * sizeof(mesh_.c[0]), mesh_.c.data(), GL_STATIC_DRAW);

	// Get normals, setup VBO
	genNormals(mesh_);
	glGenBuffers(1, &normal_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
	glBufferData(GL_ARRAY_BUFFER, mesh_.n.size() * sizeof(mesh_.n[0]), mesh_.n.data(), GL_STATIC_DRAW);

	// Unbind VAO, VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void App::InitFont()
{
	// Load lib
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "Freetype: Could not init library" << std::endl;

	// Load font
	FT_Face face;
	if (FT_New_Face(ft, "fonts/UbuntuMono-R.ttf", 0, &face))
		std::cout << "Fretype: Failed to load font" << std::endl;

	// Set up size, alignment
	FT_Set_Pixel_Sizes(face, 0, 14);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	// For each char
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load char glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "Freetype: Failed to load Glyph: " << c << std::endl;
			continue;
		}

		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Store character for use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		chars_.insert(std::pair<GLchar, Character>(c, character));
	}

	 // Clear lib resources
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void App::InitRenderText()
{
	// Text shader program
	program_id_text_ = LoadShaders("vertex_text.shader", "fragment_text.shader");

	// VAO
	glGenVertexArrays(1, &vertex_array_id_text_);
	glBindVertexArray(vertex_array_id_text_);

	// Setup vertice VBO
	glGenBuffers(1, &vertex_buffer_text_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_text_);
	glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	// Associate vertex buffer to attribute 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	// Unbind VAO, VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Set up projection
	projection_text_ = glm::ortho(0.0f, static_cast<GLfloat>(width_), 0.0f, static_cast<GLfloat>(height_));
}

void App::InitRenderGrid()
{
	// 3D line shader
	program_id_line_ = LoadShaders("vertex_line.shader", "fragment_line.shader");

	// VAO
	glGenVertexArrays(1, &vertex_array_id_line_);
	glBindVertexArray(vertex_array_id_line_);

	// Setup vertice VBO
	genGrid(mesh_grid_, 50.0f, 50, 2, glm::vec4(0.9f, 0.9f, 0.9f, 0.4f));
	glGenBuffers(1, &vertex_buffer_line_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_line_);
	glBufferData(GL_ARRAY_BUFFER, mesh_grid_.v.size() * sizeof(mesh_grid_.v[0]), mesh_grid_.v.data(), GL_STATIC_DRAW);

	// Setup color VBO
	glGenBuffers(1, &color_buffer_line_);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_line_);
	glBufferData(GL_ARRAY_BUFFER, mesh_grid_.c.size() * sizeof(mesh_grid_.c[0]), mesh_grid_.c.data(), GL_STATIC_DRAW);

	// Unbind VAO, VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::RenderTris()
{
	// Update projections & transformations
	projection_ = glm::perspective(glm::radians(45.0f), (float)width_ / (float)height_, 0.1f, 100.0f);
	model_ = glm::eulerAngleXY(rotation_[0], rotation_[1]);
	view_ = glm::lookAt(
		camera_pos_, // Camera pos
		camera_pos_ + camera_dir_, // Camera looking at
		glm::vec3(0, 1, 0)  // Up vector
	);

	// Construct model-world-projection
	glm::mat4 mvp = projection_ * view_ * model_;

	// Bind shader
	glUseProgram(program_id_);

	// Uniforms
	GLuint mvpID = glGetUniformLocation(program_id_, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

	GLuint camposID = glGetUniformLocation(program_id_, "camera_pos");
	glUniform3fv(camposID, 1, (GLfloat*)&camera_pos_);

	GLuint dirlightID = glGetUniformLocation(program_id_, "light_dir");
	glUniform3fv(dirlightID, 1, (GLfloat*)&dir_light_);

	// Clear color, depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind VAO, VBOs
	glBindVertexArray(vertex_array_id_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);

	// Set up vertex attributes
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		4,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw
	glDrawElements(GL_TRIANGLES, mesh_.t.size(), GL_UNSIGNED_INT, (void*)0);

	// Disable vertex attributes
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Unbind VAO, VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind shader
	glUseProgram(program_id_text_);

	// Uniforms
	glUniform3f(glGetUniformLocation(program_id_text_, "textColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(program_id_text_, "projection"), 1, GL_FALSE, &projection_text_[0][0]);

	// Textures
	glActiveTexture(GL_TEXTURE0);

	// Bind VAO
	glBindVertexArray(vertex_array_id_text_);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = chars_[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_text_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void App::RenderGrid()
{
	// Construct model-world-projection
	glm::mat4 mvp = projection_ * view_ * model_;

	// Bind shader
	glUseProgram(program_id_line_);

	// Uniforms
	GLuint mvpID = glGetUniformLocation(program_id_, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

	GLuint camposID = glGetUniformLocation(program_id_, "camera_pos");
	glUniform3fv(camposID, 1, (GLfloat*)&camera_pos_);

	// Bind VAO, VBO
	glBindVertexArray(vertex_array_id_line_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_line_);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_line_);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_line_);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_line_);
	glVertexAttribPointer(
		1,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_LINES, 0, mesh_grid_.v.size());

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::RenderDebug()
{
	int sizeX, sizeY;
	glfwGetWindowSize(window_, &sizeX, &sizeY);

	std::ostringstream l1, l2, l3, l4, l5;
	l1 << "Triangler version " << TRIANGLER_VERSION;
	l2 << "triangles: " << mesh_.t.size() / 3;
	l3 << "vertices:  " << mesh_.v.size();
	l4 << "viewport:  " << sizeX << "x" << sizeY;
	l5 << "framerate: " << framerate_;

	RenderText(l1.str(), 4, height_ - 14, 1.f, glm::vec3(1, 1, 1));
	RenderText(l2.str(), 4, height_ - 30, 1.f, glm::vec3(1, 1, 1));
	RenderText(l3.str(), 4, height_ - 46, 1.f, glm::vec3(1, 1, 1));
	RenderText(l4.str(), 4, height_ - 62, 1.f, glm::vec3(1, 1, 1));
	RenderText(l5.str(), 4, height_ - 78, 1.f, glm::vec3(1, 1, 1));
}
