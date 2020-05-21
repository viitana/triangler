#include <iostream>
#include <sstream>

#include "app.hpp"
#include "util.hpp"

// obj destructor
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
// intersect, genray
// proper debug ray init
// camera straight up/down
// indexed, nonindexed types to objs (enum?)
// separate all headers and implementations
// point draw support
// unify projection calc for all render steps
// don't regenerate VAOs on data change/improve init stuff
// grid draw toggle affects degub line color
// indexed normals
// notifydirty on uniform creation
// rm unneeded attach method repetition; remove pure virtual (unless we want different containers in future?)
// rm uniforms applyto, implement directly in clean()
// add override flags

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

	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);

	InitShaders();
	InitDefaultAssets();
	InitTestAssets();

	InitFont();
	InitRenderText();
	InitRenderFocuser(); // TODO: get rid of this
	InitRenderGrid();
	InitRenderDebug();
	InitGUI();

	return true;
}

void App::Run()
{
	do {
		// Framerate timing
		CheckTiming();

		// Keyboard movement
		WASDMove();

		// Clear color, depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPointSize(10);

		// Aoply render mode
		if (config_[TRIANGLER_SETTING_ID_WIREFRAME].GetBool())
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		UpdateGlobalUniforms();
		CleanUniforms();

		RenderMain();
		RenderMainLines();

		// Grid render
		if (config_[TRIANGLER_SETTING_ID_GRID_DRAW].GetBool())
			RenderGrid();

		if (config_[TRIANGLER_SETTING_ID_DEBUG].GetBool())
			RenderDebug();

		//RenderPoints();
		RenderPoints(focuser_->axis_points_);

		// Debug text render
		RenderStats();

		// Swap buffers
		glfwSwapBuffers(window_);
		glfwPollEvents();

	}
	while (!glfwWindowShouldClose(window_));

	// Cleanup VBO
	//for (auto obj : objs_)
	//	obj->ClearBuffers();

	//glDeleteProgram(program_id_);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

void App::InitShaders()
{
	uniforms_.insert({ "VP", new ShaderUniform<glm::mat4>("VP", glm::mat4(1.f)) });
	uniforms_.insert({ "camera_pos", new ShaderUniform<glm::vec3>("camera_pos", camera_.Position()) });
	uniforms_.insert({ "light_dir", new ShaderUniform<glm::vec3>("light_dir", dir_light_) });
	uniforms_.insert({ "flat_shading", new ShaderUniform<int>("flat_shading", config_[TRIANGLER_SETTING_ID_FLAT].GetBool() ? 1 : 0) });

	// Shaders
	shaders_.insert({ "main", new Shader("vertex.shader", "fragment.shader") });
	shaders_.insert({ "line", new Shader("vertex_line.shader", "fragment_line.shader") });
	shaders_.insert({ "point", new Shader("vertex_point.shader", "fragment_point.shader") });

	shaders_["main"]->Attach(uniforms_["VP"]);
	shaders_["main"]->Attach(uniforms_["camera_pos"]);
	shaders_["main"]->Attach(uniforms_["light_dir"]);
	shaders_["main"]->Attach(uniforms_["flat_shading"]);

	shaders_["point"]->Attach(uniforms_["VP"]);

}

void App::InitTestAssets()
{
	Object3D* obj1 = new Object3D(shaders_["main"]);
	LoadOBJf(obj1, "assets/bunny_lores.obj");
	InitObject(obj1);
	obj1->Translate({ -0.3f, 0, 0 });
	objs_.push_back(obj1);

	Object3D* obj2 = new Object3D(shaders_["main"]);
	loadIcosphere(obj2, 0);
	obj2->Scale(0.08f);
	obj2->Translate({ 0, 0, -0.25f });
	InitObject(obj2);
	objs_.push_back(obj2);

	Object3D* obj3 = new Object3D(shaders_["main"]);
	loadIcosphere(obj3, 3);
	InitObject(obj3);
	obj3->Scale(0.081f);
	obj3->Translate({ 0.3f, 0, 0 });
	objs_.push_back(obj3);

	Object3D* obj4 = new Object3D(shaders_["main"]);
	genRing(obj4, 128, { 1, 1, 1, 1 });
	InitRenderLineObject(obj4);
	obj4->Translate({ 0.3f, 0, 0 });
	obj4->Scale(0.1f);
	objs_line_.push_back(obj4);

	Object3D* obj5 = new Object3D(shaders_["main"]);
	LoadOBJf(obj5, "assets/bunny_lores.obj");
	obj5->Init();
	obj5->SetMesh(obj5->mesh);
	obj5->Translate({ 0, 0, 0.25f });
	objects_.push_back(obj5);

	//Object3D* grid = new Object3D();
	//genGrid(grid->mesh, 31.0f, 31, 2, glm::vec4(0.6f, 0.9f, 0.9f, 0.4f));
	//InitRenderLineObject(grid);
	//objs_line_.push_back(grid);
}

void App::InitDefaultAssets()
{
	focuser_ = new ObjectFocuser(shaders_["line"], shaders_["point"]);
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

	Ray r = camera_.GenerateRay(xpos, ypos);
	float rt = 0;

	// Left mouse to rotate object
	if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		float min_t = std::numeric_limits<float>::max();
		Object3D* selected_obj = NULL;
		for (auto obj : objs_)
		{
			bool hit = r.IntersectSphere(obj->GetMid(), obj->mesh.radius, rt);
			if (hit && rt < min_t)
			{
				selected_obj = obj;
				min_t = rt;
			}
		}
		if (!selected_obj) return;

		//FocusObject(selected_obj);
		focuser_->FocusObject(selected_obj);

		glm::vec3 grab_current_pos = r.Origin() + r.Direction() * min_t - focuser_->focused_obj_->GetMid();

		if (mouse_left_held_)
		{
			if (min_t > 0) {
				glm::vec3 grab_current_pos_n = glm::normalize(grab_current_pos);
				glm::vec3 grab_previous_pos_n = glm::normalize(obj_grab_previous_pos_);

				glm::vec3 grab_current_pos_n_objlocal = focuser_->focused_obj_->transform_rotation * glm::vec4(grab_current_pos_n, 1.f);
				glm::vec3 grab_previous_pos_n_objlocal = focuser_->focused_obj_->transform_rotation * glm::vec4(grab_previous_pos_n, 1.f);

				glm::vec3 axis_objlocal = glm::cross(grab_current_pos_n_objlocal, grab_previous_pos_n_objlocal);
				glm::vec3 axis = focuser_->focused_obj_->DirectionToGlobal(axis_objlocal);

				const float phi = acos(glm::dot(grab_current_pos_n, grab_previous_pos_n));
				const float dot = glm::dot(grab_current_pos_n, grab_previous_pos_n);

				focuser_->focused_obj_->Rotate(-phi, axis);

				obj_grab_previous_pos_ = grab_current_pos;
			}
		}
		else
		{
			if (min_t > 0) {
				addDebugVector(
					mesh_debug_,
					focuser_->focused_obj_->GetMid(),
					grab_current_pos + focuser_->focused_obj_->GetMid(),
					glm::vec4(0, 1, 0, 1)
				);
				vert_count_ += 2;
				InitRenderDebug();

				obj_grab_previous_pos_ = grab_current_pos;
				mouse_left_held_ = true;
			}
		}	
	}
	else
	{
		mouse_left_held_ = false;
	}

	// Right mouse to rotate camera
	if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glm::vec2 delta = cursor_pos_ - pos;
		camera_.RotateX(-0.001f * delta[0]);
		camera_.RotateY(-0.001f * delta[1]);
	}

	// Middle mouse to move camera
	if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		const bool hit = r.IntersectPlane({ 0, config_[TRIANGLER_SETTING_ID_GRIDHEIGHT].GetFloat(), 0 }, { 0, 1, 0 }, rt);

		if (hit)
		{
			if (mouse_mid_held_)
			{
				if (rt > 0)
				{
					glm::vec3 heldpos = grid_held_camera_pos_ + r.Direction() * rt;
					camera_.SetPosition(grid_held_camera_pos_ + grid_held_pos_ - heldpos);
				}
			}
			else
			{
				addDebugVector(
					mesh_debug_,
					camera_.Position(),
					camera_.Position() + rt * r.Direction(),
					glm::vec4(0, 1, 0, 1)
				);
				InitRenderDebug();

				glm::vec3 heldpos = camera_.Position() + r.Direction() * rt;

				if (rt > 0)
				{
					grid_held_pos_ = camera_.Position() + r.Direction() * rt;
					grid_held_camera_pos_ = camera_.Position();
				}

				mouse_mid_held_ = true;
			}
		}
		
	}
	else
	{
		mouse_mid_held_ = false;
	}

	if (focuser_->focused_obj_)
	{
		focuser_->UpdateIndicators(r);
		InitRenderFocuser();
	}
	
	cursor_pos_ = pos;
}

void App::HandleKey(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window_, GLFW_TRUE);
		if (key == GLFW_KEY_R) {
			//glDeleteProgram(program_id_);
			//program_id_ = LoadShaders("vertex.shader", "fragment.shader");
			glDeleteProgram(program_id_text_);
			program_id_text_ = LoadShaders("vertex_text.shader", "fragment_text.shader");
			glDeleteProgram(program_id_line_);
			program_id_line_ = LoadShaders("vertex_line.shader", "fragment_line.shader");
		}
		config_.Input(key);
	}
}

void App::WASDMove()
{
	if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
		camera_.MoveZ(config_[TRIANGLER_SETTING_ID_CAMSPEED].GetFloat());
	if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
		camera_.MoveZ(-config_[TRIANGLER_SETTING_ID_CAMSPEED].GetFloat());
	if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
		camera_.MoveX(config_[TRIANGLER_SETTING_ID_CAMSPEED].GetFloat());
	if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
		camera_.MoveX(-config_[TRIANGLER_SETTING_ID_CAMSPEED].GetFloat());
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
	camera_.MoveZ(0.05f * (float)yoffset);
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

// Load object mesh, generate normals and colors,
// bind data to render buffers
void App::InitObject(Object3D* obj)
{
	// VAO
	glGenVertexArrays(1, &obj->vertex_array_id_);
	glBindVertexArray(obj->vertex_array_id_);

	// Mesh VBO
	glGenBuffers(1, &obj->vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, obj->mesh.v.size() * sizeof(obj->mesh.v[0]), obj->mesh.v.data(), GL_STATIC_DRAW);

	// Mesh index buffer
	glGenBuffers(1, &obj->index_buffer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->mesh.t.size() * sizeof(obj->mesh.t[0]), obj->mesh.t.data(), GL_STATIC_DRAW);

	// Colors VBO
	genRandomColors(obj->mesh);
	glGenBuffers(1, &obj->color_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, obj->color_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, obj->mesh.c.size() * sizeof(obj->mesh.c[0]), obj->mesh.c.data(), GL_STATIC_DRAW);

	// Normals VBO
	genNormals(obj->mesh);
	glGenBuffers(1, &obj->normal_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, obj->normal_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, obj->mesh.n.size() * sizeof(obj->mesh.n[0]), obj->mesh.n.data(), GL_STATIC_DRAW);

	// Unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Increment drawing stats
	obj_count_++;
	tri_count_ += obj->mesh.v.size() / 3;
	vert_count_ += obj->mesh.v.size();
}

void App::InitRenderLineObject(Object3D* obj)
{
	// VAO
	glGenVertexArrays(1, &obj->vertex_array_id_);
	glBindVertexArray(obj->vertex_array_id_);

	// Mesh VBO
	glGenBuffers(1, &obj->vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, obj->mesh.v.size() * sizeof(obj->mesh.v[0]), obj->mesh.v.data(), GL_STATIC_DRAW);

	// Unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Increment drawing stats
	obj_count_++;
	vert_count_ += obj->mesh.v.size();
}

// Load up Freetype font glyphs, generate and bind character textures
void App::InitFont()
{
	glUseProgram(program_id_text_);

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

// Load text shaders, generate and bind VAO, VBO
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
	glGenVertexArrays(1, &vertex_array_id_grid_);
	glBindVertexArray(vertex_array_id_grid_);

	// Setup vertice VBO
	genGrid(mesh_grid_, 31.0f, 31, 2, glm::vec4(0.6f, 0.9f, 0.9f, 0.4f));
	glGenBuffers(1, &vertex_buffer_grid_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_grid_);
	glBufferData(GL_ARRAY_BUFFER, mesh_grid_.v.size() * sizeof(mesh_grid_.v[0]), mesh_grid_.v.data(), GL_STATIC_DRAW);

	// Setup color VBO
	glGenBuffers(1, &color_buffer_grid_);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_grid_);
	glBufferData(GL_ARRAY_BUFFER, mesh_grid_.c.size() * sizeof(mesh_grid_.c[0]), mesh_grid_.c.data(), GL_STATIC_DRAW);

	// Unbind VAO, VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::InitRenderPoints(Object3D* points)
{
	// Bind shader
	const GLuint program_id = shaders_["point"]->id_;
	glUseProgram(program_id);

	// VAO
	if (!glIsVertexArray(points->vertex_array_id_)) glGenVertexArrays(1, &points->vertex_array_id_);
	glBindVertexArray(points->vertex_array_id_);

	// Setup vertice VBO
	if (!glIsBuffer(points->vertex_buffer_id)) glGenBuffers(1, &points->vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, points->vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, points->mesh.v.size() * sizeof(points->mesh.v[0]), points->mesh.v.data(), GL_STATIC_DRAW);

	// Setup color VBO
	if (!glIsBuffer(points->color_buffer_id)) glGenBuffers(1, &points->color_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, points->color_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, points->mesh.c.size() * sizeof(points->mesh.c[0]), points->mesh.c.data(), GL_STATIC_DRAW);

	// Unbind VAO, VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::InitRenderDebug()
{
	// VAO
	if (!glIsVertexArray(vertex_array_id_debug_)) glGenVertexArrays(1, &vertex_array_id_debug_);
	glBindVertexArray(vertex_array_id_debug_);

	// Setup vertice VBO
	if (!glIsBuffer(vertex_buffer_debug_)) glGenBuffers(1, &vertex_buffer_debug_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_debug_);
	glBufferData(GL_ARRAY_BUFFER, mesh_debug_.v.size() * sizeof(mesh_debug_.v[0]), mesh_debug_.v.data(), GL_STATIC_DRAW);

	// Setup color VBO
	if (!glIsBuffer(color_buffer_debug_)) glGenBuffers(1, &color_buffer_debug_);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_debug_);
	glBufferData(GL_ARRAY_BUFFER, mesh_debug_.c.size() * sizeof(mesh_debug_.c[0]), mesh_debug_.c.data(), GL_STATIC_DRAW);

	// Unbind VAO, VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::InitRenderFocuser()
{
	InitRenderPoints(focuser_->axis_points_);
	for (Object3D*& ring : focuser_->axis_rings_)
	{
		InitRenderLineObject(ring);
	}
}

void App::RenderMain()
{
	for (Object3D* obj : objs_)
		RenderTris(obj);

	for (Object3D* obj : objects_)
		RenderObject(obj);
}

void App::RenderMainLines()
{
	for (Object3D* obj : objs_line_)
		RenderLines(obj);

	for (Object3D* obj : focuser_->axis_rings_)
		RenderLines(obj);
}

void App::RenderObject(Object3D* obj)
{
	obj->Render();
}

void App::RenderTris(const Object3D* obj)
{
	// Update projections & transformations
	projection_main_ = camera_.Projection((float)width_, (float)height_);
	view_main_ = camera_.View();

	const glm::mat4 model = obj->GetTransform();
	const int flat = config_[TRIANGLER_SETTING_ID_FLAT].GetBool() ? 1 : 0;

	// Model-view-projection
	const glm::mat4 vp = projection_main_ * view_main_;

	// Bind shader
	const GLuint program_id = shaders_["main"]->id_;
	glUseProgram(program_id);

	// Uniforms
	GLuint flatID = glGetUniformLocation(program_id, "flat_shading");
	glUniform1i(flatID, flat);

	GLuint modelID = glGetUniformLocation(program_id, "model_to_world");
	glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(model));

	//GLuint mvpID = glGetUniformLocation(program_id, "VP");
	//glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(vp));

	GLuint camposID = glGetUniformLocation(program_id, "camera_pos");
	glUniform3fv(camposID, 1, (GLfloat*)&camera_.Position());

	GLuint dirlightID = glGetUniformLocation(program_id, "light_dir");
	glUniform3fv(dirlightID, 1, (GLfloat*)&dir_light_);

	// Bind VAO, index
	glBindVertexArray(obj->vertex_array_id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->index_buffer_id);

	// Set up vertex attributes
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vertex_buffer_id);
	glVertexAttribPointer(
		0,                  // attribute pos in shader
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, obj->color_buffer_id);
	glVertexAttribPointer(
		1,                                // attribute pos in shader
		4,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, obj->normal_buffer_id);
	glVertexAttribPointer(
		2,                                // attribute pos in shader
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw
	glDrawElements(GL_TRIANGLES, obj->mesh.t.size(), GL_UNSIGNED_INT, (void*)0);

	// Disable vertex attributes
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Unbind VAO, VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::RenderLines(const Object3D* obj)
{
	// Update projections & transformations
	projection_main_ = camera_.Projection((float)width_, (float)height_);
	view_main_ = camera_.View();

	const glm::mat4 model = obj->GetTransform();

	// Model-view-projection
	const glm::mat4 mvp = projection_main_ * view_main_ * model;

	// Bind shader
	glUseProgram(program_id_line_);

	// Uniforms
	GLuint mvpID = glGetUniformLocation(program_id_line_, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(mvp));

	GLuint camposID = glGetUniformLocation(program_id_line_, "camera_pos");
	glUniform3fv(camposID, 1, (GLfloat*)&camera_.Position());

	GLuint linecolorID = glGetUniformLocation(program_id_line_, "line_color");
	glUniform4fv(linecolorID, 1, (GLfloat*)&obj->line_color);

	GLuint multicolorID = glGetUniformLocation(program_id_line_, "multi_color");
	glUniform1i(multicolorID, 0);

	// Bind VAO, VBO
	glBindVertexArray(obj->vertex_array_id_);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vertex_buffer_id);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vertex_buffer_id);
	glVertexAttribPointer(
		0,                  // attribute pos in shader
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_LINES, 0, obj->mesh.v.size());

	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::RenderPoints(Object3D* points)
{
	const glm::mat4 mvp = projection_main_ * view_main_;

	// Bind shader
	const GLuint program_id = shaders_["point"]->id_;
	glUseProgram(program_id);

	GLuint mvpID = glGetUniformLocation(program_id, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(mvp));

	// Bind VAO, index
	glBindVertexArray(points->vertex_array_id_);

	// Set up vertex attributes
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, points->vertex_buffer_id);
	glVertexAttribPointer(
		0,                  // attribute pos in shader
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Set up vertex attributes
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, points->color_buffer_id);
	glVertexAttribPointer(
		1,                  // attribute pos in shader
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw
	glDrawArrays(GL_POINTS, 0, points->mesh.v.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	glEnable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind shader
	glUseProgram(program_id_text_);

	// Uniforms
	glUniform3f(glGetUniformLocation(program_id_text_, "textColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(program_id_text_, "projection"), 1, GL_FALSE, glm::value_ptr(projection_text_));

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
	// Retrieve grid height from config
	auto height = config_[TRIANGLER_SETTING_ID_GRIDHEIGHT].GetFloat();

	// Model-view-projection
	glm::mat4 mvp = projection_main_ * view_main_ * glm::translate(glm::vec3(0, height,0)) * glm::mat4(1.0);

	// Bind shader
	glUseProgram(program_id_line_);

	// Uniforms
	GLuint mvpID = glGetUniformLocation(program_id_line_, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(mvp));

	GLuint camposID = glGetUniformLocation(program_id_line_, "camera_pos");
	glUniform3fv(camposID, 1, (GLfloat*)&camera_.Position());

	GLuint multicolorID = glGetUniformLocation(program_id_line_, "multi_color");
	glUniform1i(multicolorID, 1);

	// Bind VAO
	glBindVertexArray(vertex_array_id_grid_);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_grid_);
	glVertexAttribPointer(
		0,                  // attribute pos in shader
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_grid_);
	glVertexAttribPointer(
		1,                  // attribute pos in shader
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
	// Model-view-projection
	glm::mat4 mvp = projection_main_ * view_main_ * glm::mat4(1.0);

	// Bind shader
	glUseProgram(program_id_line_);

	// Uniforms
	GLuint mvpID = glGetUniformLocation(program_id_line_, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(mvp));

	GLuint camposID = glGetUniformLocation(program_id_line_, "camera_pos");
	glUniform3fv(camposID, 1, (GLfloat*)&camera_.Position());

	// Bind VAO, VBO
	glBindVertexArray(vertex_array_id_debug_);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_debug_);
	glVertexAttribPointer(
		0,                  // attribute pos in shader
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_debug_);
	glVertexAttribPointer(
		1,                  // attribute pos in shader
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_LINES, 0, mesh_debug_.v.size());

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void App::RenderStats()
{
	int sizeX, sizeY;
	glfwGetWindowSize(window_, &sizeX, &sizeY);

	std::ostringstream l1, l2, l3, l4, l5;
	l1 << "Triangler version " << TRIANGLER_VERSION;
	l2 << "triangles: " << tri_count_;
	l3 << "vertices:  " << vert_count_;
	l4 << "viewport:  " << sizeX << "x" << sizeY;
	l5 << "framerate: " << framerate_;

	RenderText(l1.str(), 4, height_ - 14, 1.f, glm::vec3(1, 1, 1));
	RenderText(l2.str(), 4, height_ - 30, 1.f, glm::vec3(1, 1, 1));
	RenderText(l3.str(), 4, height_ - 46, 1.f, glm::vec3(1, 1, 1));
	RenderText(l4.str(), 4, height_ - 62, 1.f, glm::vec3(1, 1, 1));
	RenderText(l5.str(), 4, height_ - 78, 1.f, glm::vec3(1, 1, 1));

	const auto menu = config_.GetMenu();
	const unsigned index = config_.GetIndex();
	for (unsigned i = 0; i < menu.size(); i++)
	{
		glm::vec3 color = { 1, 1, 1 };
		if (index == i) color.b = 0;
		RenderText(menu[i], 24, (24 + i * 16), 1.f, color);
	}

}

void App::InitGUI()
{

}

void App::RenderGUI()
{
	
}

void App::UpdateGlobalUniforms()
{
	// View-projection
	static_cast<ShaderUniform<glm::mat4>*>(uniforms_["VP"])->SetValue(projection_main_ * view_main_);
}

void App::CleanUniforms()
{
	for (const auto& pair : shaders_)
	{
		Shader* shader = pair.second;
		shader->CleanObservees();
	}
}
