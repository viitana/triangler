#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <queue> 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H  

#include "shader.hpp"
#include "mesh.hpp"
#include "font.hpp"

#define TRIANGLER_VERSION "0.0.1"
#define WINDOW_START_WIDTH 1280
#define WINDOW_START_HEIGHT 800

class App
{
public:
	App() {}
	bool Initialize();
	bool InitGLFW();
	bool InitGLEW();
	void InitRenderMain();
	void InitFont();
	void InitRenderText();
	void InitRenderGrid();

	void Run();
	void CheckTiming();
	void WASDMove();

	void HandleCursorMove(double xpos, double ypos);
	void HandleResize(int height, int width);
	void HandleMouseScroll(double xoffset, double yoffset);
	void HandleKey(int key, int scancode, int action, int mods);

	void RenderTris();
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	void RenderDebug();
	void RenderGrid();
private:
	GLFWwindow* window_;

	GLuint program_id_;
	GLuint program_id_text_;
	GLuint program_id_line_;

	GLuint vertex_array_id_;
	GLuint vertex_buffer_;
	GLuint index_buffer_;
	GLuint color_buffer_;
	GLuint normal_buffer_;

	GLuint vertex_array_id_text_;
	GLuint vertex_buffer_text_;

	GLuint vertex_array_id_line_;
	GLuint vertex_buffer_line_;
	GLuint color_buffer_line_;

	glm::mat4 projection_text_;
	unsigned height_ = WINDOW_START_HEIGHT;
	unsigned width_ = WINDOW_START_WIDTH;
	glm::vec3 camera_pos_ = { 0, 2, 5};
	glm::vec3 camera_dir_ = { 0, 0, -5};
	glm::mat4 projection_;
	glm::mat4 view_;
	glm::mat4 model_;
	glm::vec3 dir_light_ = { 1.2f, 1.f, -4.f };
	glm::vec2 cursor_pos_ = { -1, -1 };
	glm::vec3 rotation_ = { 0, 0, 0 }; // Euler angles
	std::vector<glm::vec3> color_buffer_data_;
	std::vector<glm::vec3> vertex_buffer_data_;
	std::vector<glm::vec3> normal_buffer_data_;
	std::vector<glm::vec3> grid_vertex_buffer_data_;
	Mesh mesh_;
	Mesh mesh_grid_;
	std::map<GLchar, Character> chars_;

	float grid_height_ = 0.0f;
	float camera_speed_ = 0.01f;

	// Grid grab tracking
	bool mouse_mid_held_ = false;
	glm::vec3 grid_held_pos_;
	glm::vec3 grid_held_camera_pos_;

	// Frametime tracking
	static constexpr int frametime_buffer_size_ = 100;
	std::queue<double> frametimes_;
	float framerate_;
};
