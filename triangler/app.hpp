#pragma once

#define IMGUI_IMPL_OPENGL_LOADER_GLEW

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <queue> 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H  

#include "shader.hpp"
#include "mesh.hpp"
#include "font.hpp"
#include "camera.hpp"
#include "object3d.hpp"
#include "menu.hpp"

#define TRIANGLER_VERSION "0.0.2"
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
	void InitObject(Object3D* obj);
	void BindObject(const Object3D* obj);

	void Run();
	void CheckTiming();
	void WASDMove();

	void HandleCursorMove(double xpos, double ypos);
	void HandleResize(int height, int width);
	void HandleMouseScroll(double xoffset, double yoffset);
	void HandleKey(int key, int scancode, int action, int mods);

	void RenderMain();
	void RenderTris(const Object3D* obj);
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	void RenderDebug();
	void RenderGrid();

	void InitGUI();
	void RenderGUI();
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
	glm::mat4 projection_main_;
	glm::mat4 view_main_;
	glm::vec3 dir_light_ = { 1.2f, 1.f, -4.f };
	glm::vec2 cursor_pos_ = { -1, -1 };

	Object3D obj_selected_;
	std::vector<Object3D*> objs_;

	TrianglerConfig config_;

	Mesh mesh_grid_;
	std::map<GLchar, Character> chars_;

	Camera camera_ = { {0, 1, 1}, {0, -1, -1}, 45.f, .05f, 100.f };

	// Grid grab tracking
	bool mouse_mid_held_ = false;
	glm::vec3 grid_held_pos_;
	glm::vec3 grid_held_camera_pos_;

	// Object grab tracking
	bool mouse_left_held_ = false;
	glm::vec3 obj_grab_previous_pos_;

	// Frametime tracking
	static constexpr int frametime_buffer_size_ = 100;
	std::queue<double> frametimes_;
	float framerate_;

	unsigned long tri_count_ = 0u;
	unsigned long vert_count_ = 0u;
	unsigned long obj_count_ = 0u;

	// Camera/perspective properties
	glm::vec3 u_;
	glm::vec3 v_;
	glm::vec3 w_;
};
