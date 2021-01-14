#include <Runtime/Runtime.hpp>
#include <glad/gl.h>
#include <glfw-3.3.2/include/GLFW/glfw3.h>
#include <Runtime/Debug.hpp>
#include <Runtime/Math.hpp>
#include "Camera.hpp"
#include <stb_image/stb_image.h>
#include <TinyObjLoader/tiny_obj_loader.h>
#include <Runtime/Vector.hpp>
namespace Luna
{
	Camera camera;

	void error_callback(int error, const char* desc)
	{
		debug_printf("Error: %s\n", desc);
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		camera.aspect_ratio = ((f32)width / (f32)height);
	}

	void check_shader_compile_error(GLuint shader, const c8* type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				debug_printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				debug_printf("ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
			}
		}
	}

	void* load_file(const c8* filepath, u32& size)
	{
		FILE* file = fopen(filepath, "rb");
		if (!file)
		{
			return nullptr;
		}
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);
		char* buf = (char*)malloc(size);
		fread(buf, size, 1, file);
		fclose(file);
		return buf;
	}

	GLint load_program(const c8* vert_path, const c8* frag_path)
	{
		u32 vert_sz, frag_sz;
		c8* vert = (c8*)load_file(vert_path, vert_sz);
		c8* frag = (c8*)load_file(frag_path, frag_sz);
		if (!vert || !frag)
		{
			if (!vert)
			{
				debug_printf("ERROR::SHADER_LOADING_ERROR: Cannot load vertex shader %s\n", vert_path);
			}
			else
			{
				free(vert);
			}
			if (!frag)
			{
				debug_printf("ERROR::SHADER_LOADING_ERROR: Cannot load fragment shader %s\n", frag_path);
			}
			else
			{
				free(frag);
			}
			return 0;
		}
		GLuint vertex, fragment;
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vert, (GLint*)&vert_sz);
		glCompileShader(vertex);
		check_shader_compile_error(vertex, "VERTEX");
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &frag, (GLint*)&frag_sz);
		glCompileShader(fragment);
		check_shader_compile_error(fragment, "FRAGMENT");
		GLuint id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);
		check_shader_compile_error(id, "PROGRAM");
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return id;
	}

	GLuint load_texture(const c8* filepath)
	{
		u32 size = 0;
		void* buf = load_file(filepath, size);
		if (!buf)
		{
			debug_printf("ERROR::TEXTURE_LOADING_ERROR: Cannot load texture %s\n", filepath);
			return 0;
		}
		GLuint textureId = 0;
		int channel = 0;
		int width, height;
		stbi_uc* data = stbi_load_from_memory((stbi_uc const*)buf, size, &width, &height, &channel,
			STBI_rgb_alpha);
		if (data != nullptr)
		{
			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		free(buf);
		return textureId;
	}

	struct Vertex
	{
		Float3U position;
		Float3U normal;
		Float3U tangent;
		Float2U texcoord;
	};

	void load_model(const c8* filepath, GLuint& vao, GLuint& vbo, GLuint& ibo, usize& num_vertices, usize& num_indices)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> meterials;
		std::string warn;
		std::string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &meterials, &warn, &err, filepath);

		if (!ret)
		{
			if (!err.empty())
			{
				debug_printf("ERROR::MODEL_LOADING_ERROR: %s", err.c_str());
			}
			vao = 0;
			vbo = 0;
			ibo = 0;
			return;
		}


		// Generating vertex lists.
		Vector<Vertex> vertices;
		for (auto& i : shapes[0].mesh.indices)
		{
			Vertex v;
			v.position = Float3U(attrib.vertices[i.vertex_index * 3], attrib.vertices[i.vertex_index * 3 + 1], attrib.vertices[i.vertex_index * 3 + 2]);
			if (i.normal_index != -1 && i.normal_index * 3 < attrib.normals.size())
			{
				v.normal = Float3U(attrib.normals[i.normal_index * 3], attrib.normals[i.normal_index * 3 + 1], attrib.normals[i.normal_index * 3 + 2]);
			}
			else
			{
				v.normal = Float3U(0.0f, 0.0f, 1.0f);
			}
			//v.normal = Float3U(0.0f, 0.0f, 1.0f);
			if (i.texcoord_index != -1 && i.texcoord_index * 2 < attrib.texcoords.size())
			{
				v.texcoord = Float2U(attrib.texcoords[i.texcoord_index * 2], attrib.texcoords[i.texcoord_index * 2 + 1]);
			}
			else
			{
				v.texcoord = Float2U(0.0f, 0.0f);
			}
			v.tangent = Float3U(0.0f, 0.0f, 0.0f);
			vertices.push_back(v);
		}

		// Generating index lists.
		Vector<u32> indices;
		usize vert_offset = 0;
		auto& faces = shapes[0].mesh.num_face_vertices;
		for (usize i = 0; i < faces.size(); ++i)
		{
			for (usize j = 0; j < (faces[i] - 2); ++j)
			{
				indices.push_back(vert_offset);
				indices.push_back(vert_offset + j + 2);
				indices.push_back(vert_offset + j + 1);
			}
			vert_offset += faces[i];
		}

		usize num_tris = indices.size() / 3;

		// Calculate tangents.
		for (usize j = 0; j < num_tris; ++j)
		{
			u32 i1 = indices[j * 3];
			u32 i2 = indices[j * 3 + 1];
			u32 i3 = indices[j * 3 + 2];
			Vertex& a = vertices[i1];
			Vertex& b = vertices[i2];
			Vertex& c = vertices[i3];

			auto ab = b.position - a.position;
			auto ac = c.position - a.position;
			auto bc = c.position - b.position;

			auto uab = b.texcoord.x - a.texcoord.x;
			auto vab = b.texcoord.y - a.texcoord.y;
			auto uac = c.texcoord.x - a.texcoord.x;
			auto vac = c.texcoord.y - a.texcoord.y;
			auto ubc = c.texcoord.x - b.texcoord.x;
			auto vbc = c.texcoord.y - b.texcoord.y;

			auto ta = (ac * vab - ab * vac) / (uac * vab - uab * vac);
			auto tb = (bc * vab - ab * vbc) / (ubc * vab - uab * vbc);
			auto tc = (bc * vac - ac * vbc) / (ubc * vac - uac * vbc);

			a.tangent = a.tangent + Float3U(ta);
			b.tangent = b.tangent + Float3U(tb);
			c.tangent = c.tangent + Float3U(tc);
		}


		for (size_t i = 0; i < vertices.size(); ++i)
		{
			Float3 n = vertices[i].normal;
			Float3 t = vertices[i].tangent;

			// Gram-Schmidt orthogonalize
			Float3 tang = normalize(t - n * dot(n, t));
			vertices[i].tangent = tang;
		}

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

		// position.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)0);
		glEnableVertexAttribArray(0);
		// normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(3 * sizeof(f32)));
		glEnableVertexAttribArray(1);
		// tangent
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(6 * sizeof(f32)));
		glEnableVertexAttribArray(2);
		// texcoord
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(9 * sizeof(f32)));
		glEnableVertexAttribArray(3);

		num_vertices = vertices.size();
		num_indices = indices.size();

	}

	struct Light
	{
		Float3U position;
		Float3U color;
		f32 intensity;
	};

	Light lights[8];
	Tranform3D model_transform = Tranform3D::identity();
	
	void run()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwSetErrorCallback(error_callback);
		GLFWwindow* window = glfwCreateWindow(800, 600, "Exp02", NULL, NULL);
		luassert_always(window);
		glfwSetKeyCallback(window, key_callback);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwMakeContextCurrent(window);
		luassert_always(gladLoadGL(glfwGetProcAddress));

		// Load all programs.
		GLuint program = load_program("LightingVS.glsl", "LightingFS.glsl");
		// Load all textures.
		GLuint diffuse_tex = load_texture("mask01_lvl01_defaultMat_BaseColor.png");
		GLuint normal_tex = load_texture("mask01_lvl01_defaultMat_Normal.png");
		GLuint specular_tex = load_texture("mask01_lvl01_defaultMat_Specular.png");
		// Load model.
		GLuint vao, vbo, ibo;
		usize num_vertices, num_indices;
		load_model("mask01_lvl01.OBJ", vao, vbo, ibo, num_vertices, num_indices);

		camera.transform.position = Float3(0.0f, 0.0f, 1.0f);
		camera.transform.rotation = Quaternion::from_axis_angle(Float3(0.0f, 1.0f, 0.0f), pi);

		bool mouse_pressed = false;
		Float2 mouse_click_pos;
		f32 model_rot = 0.0f;

		lights[0] = { {0.0f, 1.0f, 3.0f}, {1.0f, 1.0f, 1.0f}, 5.0f };
		lights[1] = { {-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, 2.5f };
		lights[2] = { {1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, 2.5f };
		lights[3] = { {1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 0.3f };
		lights[4] = { {1.0f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, 0.3f };
		lights[5] = { {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, 0.3f };
		lights[6] = { {1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, 0.3f };
		lights[7] = { {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, 0.3f };

		f32 lights_rot[5] = { 0.0f };
		f32 lights_speed[5] = { 1.3f, 2.21f, -0.78f, -1.86f, 1.75f };

		while (!glfwWindowShouldClose(window))
		{
			{
				if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
				{
					auto camera_vector = mul(Float4(0.0f, 0.0f, -camera.focus_distance, 1.0f),
						Float4x4::make_rotation_quaternion(camera.transform.rotation));
					camera.transform.position = Float3(camera_vector.x, camera_vector.y, camera_vector.z);
				}
				if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
				{
					// Inspect mode.
					if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) || 
						(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) ||
						(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS))
					{
						if (!mouse_pressed)
						{
							mouse_pressed = true;
							double x, y;
							glfwGetCursorPos(window, &x, &y);
							mouse_click_pos = Float2((f32)x, (f32)y);
							
						}
						double x, y;
						glfwGetCursorPos(window, &x, &y);
						Float2 mouse_pos = Float2((f32)x, (f32)y);
						auto mouse_delta = mouse_pos - mouse_click_pos;
						glfwSetCursorPos(window, mouse_click_pos.x, mouse_click_pos.y);

						if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
						{
							// Rotate.
							auto focus_vector = mul(Float4(0.0f, 0.0f, camera.focus_distance, 1.0f), Float4x4::make_rotation_quaternion(camera.transform.rotation));
							auto focus_point = camera.transform.position + Float3(focus_vector.x, focus_vector.y, focus_vector.z);
							auto rot_mat = Float4x4::make_rotation_quaternion(camera.transform.rotation);
							auto euler = rot_mat.euler_angles();
							euler += {deg_to_rad((f32)mouse_delta.y / 10.0f), deg_to_rad((f32)mouse_delta.x / 10.0f), 0.0f};
							euler.x = clamp(euler.x, deg_to_rad(-85.0f), deg_to_rad(85.0f));
							camera.transform.rotation = Quaternion::from_roll_pitch_yaw(euler.x, euler.y, euler.z);
							auto camera_vector = mul(Float4(0.0f, 0.0f, -camera.focus_distance, 1.0f),
								Float4x4::make_rotation_quaternion(camera.transform.rotation));
							camera.transform.position = focus_point + Float3(camera_vector.x, camera_vector.y, camera_vector.z);
						}
						else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
						{
							// Pan.
							constexpr f32 camera_speed = 0.01f;
							auto rot_mat = Float4x4::make_rotation_quaternion(camera.transform.rotation);
							auto left = rot_mat.left();
							auto up = rot_mat.up();
							camera.transform.position += left * camera_speed * mouse_delta.x + up * camera_speed * mouse_delta.y;
						}
						else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS)
						{
							// Zoom.
							constexpr f32 camera_speed = 0.01f;
							auto focus_vector = mul(Float4(0.0f, 0.0f, camera.focus_distance, 1.0f), Float4x4::make_rotation_quaternion(camera.transform.rotation));
							auto focus_point = camera.transform.position + Float3(focus_vector.x, focus_vector.y, focus_vector.z);
							camera.focus_distance *= max(1.0f + camera_speed * mouse_delta.y, 0.5f);
							auto camera_vector = mul(Float4(0.0f, 0.0f, -camera.focus_distance, 1.0f),
								Float4x4::make_rotation_quaternion(camera.transform.rotation));
							camera.transform.position = focus_point + Float3(camera_vector.x, camera_vector.y, camera_vector.z);
						}
					}
				}
				else
				{
					// Free pilot mode.
					constexpr f32 camera_speed = 0.01f;
					auto rot_mat = Float4x4::make_rotation_quaternion(camera.transform.rotation);
					auto left = rot_mat.left();
					auto forward = rot_mat.forward();
					auto up = rot_mat.up();
					if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
					{
						camera.transform.position += forward * camera_speed;
					}
					if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
					{
						camera.transform.position -= forward * camera_speed;
					}
					if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
					{
						camera.transform.position += left * camera_speed;
					}
					if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
					{
						camera.transform.position -= left * camera_speed;
					}
					if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
					{
						camera.transform.position += up * camera_speed;
					}
					if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
					{
						camera.transform.position -= up * camera_speed;
					}
					if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
					{
						if (!mouse_pressed)
						{
							mouse_pressed = true;
							double x, y;
							glfwGetCursorPos(window, &x, &y);
							mouse_click_pos = Float2((f32)x, (f32)y);
						}
						double x, y;
						glfwGetCursorPos(window, &x, &y);
						Float2 mouse_pos = Float2((f32)x, (f32)y);
						auto mouse_delta = mouse_pos - mouse_click_pos;
						glfwSetCursorPos(window, mouse_click_pos.x, mouse_click_pos.y);
						auto euler = rot_mat.euler_angles();
						euler += {deg_to_rad((f32)mouse_delta.y / 10.0f), deg_to_rad((f32)mouse_delta.x / 10.0f), 0.0f};
						euler.x = clamp(euler.x, deg_to_rad(-85.0f), deg_to_rad(85.0f));
						camera.transform.rotation = Quaternion::from_roll_pitch_yaw(euler.x, euler.y, euler.z);
					}
				}
				if ((!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) && 
					(!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) &&
					(!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS))
				{
					mouse_pressed = false;
				}
			}
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			model_transform.rotation = Quaternion::from_axis_angle(Float3(0.0f, 1.0f, 0.0f), model_rot);
			model_rot += pi / 720.0f;
			for (u32 i = 0; i < 5; ++i)
			{
				lights_rot[i] += lights_speed[i] * pi / 60.0f;
				f32 len = length(Float2(lights[i + 3].position.x, lights[i + 3].position.z));
				lights[i + 3].position.x = sinf(lights_rot[i]) * len;
				lights[i + 3].position.z = cosf(lights_rot[i]) * len;
			}

			Float4x4 model = Float4x4::make_affine_position_rotation_scale(model_transform.position, model_transform.rotation, model_transform.scale);
			Float4x4 view = camera.view_matrix();
			Float4x4 proj = camera.projection_matrix();

			glUseProgram(program);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &proj[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(program, "world_to_model"), 1, GL_FALSE, &inverse(model)[0][0]);
			glUniform3f(glGetUniformLocation(program, "camera_pos"), camera.transform.position.x, camera.transform.position.y, camera.transform.position.z);


			Float3U light_pos[8];
			Float3U light_color[8];
			f32 light_intensity[8];
			for (u32 i = 0; i < 8; ++i)
			{
				light_pos[i] = lights[i].position;
				light_color[i] = lights[i].color;
				light_intensity[i] = lights[i].intensity;
			}

			glUniform3fv(glGetUniformLocation(program, "light_pos"), 8, &light_pos[0].x);
			glUniform3fv(glGetUniformLocation(program, "light_color"), 8, &light_color[0].x);
			glUniform1fv(glGetUniformLocation(program, "light_intensity"), 8, light_intensity);
			glUniform1i(glGetUniformLocation(program, "diffuse_tex"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuse_tex);
			glUniform1i(glGetUniformLocation(program, "specular_tex"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specular_tex);
			glUniform1i(glGetUniformLocation(program, "normal_tex"), 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, normal_tex);

			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

			glFlush();

			glfwSwapBuffers(window);
			glfwPollEvents();

		}

		glDeleteProgram(program);
		glDeleteTextures(1, &diffuse_tex);
		glDeleteTextures(1, &normal_tex);
		glDeleteTextures(1, &specular_tex);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);

		glfwDestroyWindow(window);
	}
}

int main()
{
	Luna::init();
	luassert_always(glfwInit());
	Luna::run();
	glfwTerminate();
	Luna::close();
}