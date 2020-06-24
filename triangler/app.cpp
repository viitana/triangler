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
// don't change actual mesh radius on object scale

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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	InitShaders();
	InitDefaultAssets();
	InitTestAssets();

	InitFont();
	InitRenderText();

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
	uniforms_.insert({ "projection_text", new ShaderUniform<glm::mat4>("projection_text", glm::mat4(1.f)) });
	uniforms_.insert({ "camera_pos", new ShaderUniform<glm::vec3>("camera_pos", camera_.Position()) });
	uniforms_.insert({ "light_dir", new ShaderUniform<glm::vec3>("light_dir", dir_light_) });
	uniforms_.insert({ "flat_shading", new ShaderUniform<int>("flat_shading", config_[TRIANGLER_SETTING_ID_FLAT].GetBool() ? 1 : 0) });

	// Shaders
	shaders_.insert({ "main", new Shader("vertex.shader", "fragment.shader") });
	shaders_.insert({ "line", new Shader("vertex_line.shader", "fragment_line.shader") });
	shaders_.insert({ "point", new Shader("vertex_point.shader", "fragment_point.shader") });
	shaders_.insert({ "text", new Shader("vertex_text.shader", "fragment_text.shader") });

	shaders_["main"]->Attach(uniforms_["VP"]);
	shaders_["main"]->Attach(uniforms_["camera_pos"]);
	shaders_["main"]->Attach(uniforms_["light_dir"]);
	shaders_["main"]->Attach(uniforms_["flat_shading"]);

	shaders_["line"]->Attach(uniforms_["VP"]);
	shaders_["line"]->Attach(uniforms_["camera_pos"]);

	shaders_["point"]->Attach(uniforms_["VP"]);

	shaders_["text"]->Attach(uniforms_["projection_text"]);
}

void App::InitTestAssets()
{
	//Object3D* obj1 = new Object3D(ObjectType::Mesh, shaders_["main"]);
	//obj1->Init();
	//obj1->SetMesh(LoadOBJ("assets/bunny_lores.obj"));
	////LoadOBJf(obj1, "assets/bunny_lores.obj");
	////InitObject(obj1);
	//obj1->Translate({ -0.3f, 0, 0 });
	//objects_.push_back(obj1);

	Object3D* obj2 = new Object3D(ObjectType::Mesh, shaders_["main"]);
	obj2->Init();
	obj2->SetMesh(genIcosphere(0));
	//loadIcosphere(obj2, 0);
	obj2->Scale(0.08f);
	obj2->Translate({ 0, 0, -0.25f });
	objects_.push_back(obj2);

	Object3D* obj3 = new Object3D(ObjectType::Mesh, shaders_["main"]);
	obj3->Init();
	obj3->SetMesh(genIcosphere(3));
	//loadIcosphere(obj3, 3);
	//InitObject(obj3);
	obj3->Scale(0.081f);
	obj3->Translate({ 0.3f, 0, 0 });
	objects_.push_back(obj3);

	Object3D* obj4 = new Object3D(ObjectType::Line, shaders_["line"]);
	obj4->Init();
	obj4->SetMesh(genRing(128, { 0, 1, 1, 1 }));
	obj4->Translate({ 0.3f, 0, 0 });
	obj4->Scale(0.1f);
	objects_.push_back(obj4);

	Object3D* obj5 = new Object3D(ObjectType::Mesh, shaders_["main"]);
	obj5->Init();
	obj5->SetMesh(LoadOBJFast("assets/test_cube.obj", "assets"));
	obj5->Translate({ 0, 0, 0.25f });
	obj5->Scale(.1f);
	objects_.push_back(obj5);

	Object3D* obj6 = new Object3D(ObjectType::Mesh, shaders_["main"]);
	obj6->Init();
	obj6->SetMesh(LoadOBJFast("assets/bunny_lores.obj", "assets"));
	obj6->Translate({ 0, 0, 0.5f });
	objects_.push_back(obj6);

}

void App::InitDefaultAssets()
{
	grid_ = new Object3D(ObjectType::Line, shaders_["line"]);
	grid_->Init();
	grid_->SetMesh(genGrid(31.0f, 31, 2, glm::vec4(0.6f, 0.9f, 0.9f, 0.4f)));

	focuser_ = new ObjectFocuser(shaders_["line"], shaders_["point"]);
}

void App::AddDebugVector(const glm::vec3 start, const glm::vec3 end, const glm::vec4 color)
{
	Object3D* obj = new Object3D(ObjectType::Line, shaders_["line"]);
	obj->Init();
	obj->SetMesh(genVector(start, end, color));
	obj->debug_ = true;
	objects_.push_back(obj);
	vert_count_ += 2;
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
		for (auto obj : objects_)
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
				AddDebugVector(
					focuser_->focused_obj_->GetMid(),
					grab_current_pos + focuser_->focused_obj_->GetMid(),
					glm::vec4(0, 1, 0, 1)
				);

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
				AddDebugVector(
					camera_.Position(),
					camera_.Position() + rt * r.Direction(),
					glm::vec4(1, 0, 0, 1)
				);

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
	}
	
	cursor_pos_ = pos;
}

void App::HandleKey(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window_, GLFW_TRUE);
		//if (key == GLFW_KEY_R) {
		//	
		//}
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

void App::RenderMain()
{
	for (Object3D* obj : objects_)
		if (draw_debug_ || !obj->debug_) obj->Render();

	for (Object3D* obj : focuser_->axis_rings_)
		obj->Render();

	if (config_[TRIANGLER_SETTING_ID_GRID_DRAW].GetBool())
		grid_->Render();

	focuser_->axis_points_->Render();
}

void App::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Bind shader
	glUseProgram(shaders_["text"]->id_);

	// Uniforms
	glUniform3f(glGetUniformLocation(shaders_["text"]->id_, "textColor"), color.x, color.y, color.z);

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

void App::UpdateGlobalUniforms()
{
	projection_main_ = camera_.Projection((float)width_, (float)height_);
	view_main_ = camera_.View();
	draw_debug_ = config_[TRIANGLER_SETTING_ID_DEBUG].GetBool();

	const float grid_height = config_[TRIANGLER_SETTING_ID_GRIDHEIGHT].GetFloat();
	grid_->ResetTranslation();
	grid_->Translate(glm::translate(glm::vec3(0, grid_height, 0)));

	// Update global uniform values
	static_cast<ShaderUniform<glm::mat4>*>(uniforms_["VP"])->SetValue(projection_main_ * view_main_);
	static_cast<ShaderUniform<glm::mat4>*>(uniforms_["projection_text"])->SetValue(projection_text_);
	static_cast<ShaderUniform<glm::vec3>*>(uniforms_["camera_pos"])->SetValue(camera_.Position());
	static_cast<ShaderUniform<glm::vec3>*>(uniforms_["light_dir"])->SetValue(dir_light_);
	static_cast<ShaderUniform<int>*>(uniforms_["flat_shading"])->SetValue(config_[TRIANGLER_SETTING_ID_FLAT].GetBool() ? 1 : 0);
}

void App::CleanUniforms()
{
	for (const auto& pair : shaders_)
	{
		Shader* shader = pair.second;
		shader->CleanObservees();
	}
}
