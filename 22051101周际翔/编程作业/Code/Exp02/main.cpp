#include <Runtime/Runtime.hpp>
#include <glad/gl.h>
#include <glfw-3.3.2/include/GLFW/glfw3.h>
#include <Runtime/Debug.hpp>
#include <Runtime/Math.hpp>
#include "Camera.hpp"
#include <stb_image/stb_image.h>

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

	struct SunVertex
	{
		Float3U pos;
		Float2U uv;
	};

	struct SkyBoxVertex
	{
		Float3U pos;
	};

	void generate_sphere_mesh(u32 lat, u32 lon, f32 radius, usize vertex_size, usize pos_offset, void** out_vertices, u32** out_indices, u32& out_num_vertices, u32& out_num_indices)
	{
		u32 vtx_count = lon * (lat - 2) + 2;
		u32 idx_count = 6 * lon * (lat - 2);
		out_num_vertices = vtx_count;
		out_num_indices = idx_count;
		void* vertices = malloc(vtx_count * vertex_size);
		*out_vertices = vertices;
		usize offset = pos_offset;
		*(Float3U*)((usize)vertices + offset) = Float3U(0.0f, -radius, 0.0f);
		offset += vertex_size;
		for (u32 i = 1; i < lat - 1; ++i)
		{
			for (u32 j = 0; j < lon; ++j)
			{
				f32 theta = lerp(0.0f, pi, (f32)i / (f32)(lat - 1));
				f32 theta2 = lerp(0.0f, 2 * pi, (f32)j / (f32)(lon));
				*(Float3U*)((usize)vertices + offset) = Float3U(
					sinf(theta) * radius * cosf(theta2),
					-cosf(theta) * radius,
					sinf(theta) * radius * sinf(theta2)
				);
				offset += vertex_size;
			}
		}
		*(Float3U*)((usize)vertices + offset) = Float3U(0.0f, radius, 0.0f);
		offset += vertex_size;

		u32* indices = (u32*)malloc(idx_count * sizeof(u32));
		*out_indices = indices;
		u32 idx = 0;
		for (u32 i = 0; i < lon; ++i)
		{
			indices[idx] = 0; ++idx;
			indices[idx] = i + 1; ++idx;
			indices[idx] = i + 2; ++idx;
		}
		for (u32 i = 0; i < lat - 3; ++i)
		{
			for (u32 j = 0; j < lon; ++j)
			{
				indices[idx] = 1 + lon * i + j; ++idx;
				indices[idx] = 1 + lon * (i + 1) + j; ++idx;
				indices[idx] = 2 + lon * i + j; ++idx;
				indices[idx] = 2 + lon * i + j; ++idx;
				indices[idx] = 1 + lon * (i + 1) + j; ++idx;
				indices[idx] = 2 + lon * (i + 1) + j; ++idx;
			}
		}
		u32 base_index = 1 + lon * (lat - 3);
		for (u32 i = 0; i < lon; ++i)
		{
			indices[idx] = base_index + i; ++idx;
			indices[idx] = vtx_count - 1; ++idx;
			indices[idx] = base_index + i + 1; ++idx;
		}
	}

	void run()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwSetErrorCallback(error_callback);
		GLFWwindow* window = glfwCreateWindow(800, 600, "Exp01", NULL, NULL);
		luassert_always(window);
		glfwSetKeyCallback(window, key_callback);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwMakeContextCurrent(window);
		luassert_always(gladLoadGL(glfwGetProcAddress));

		// Load all programs.
		//GLuint sun_program = load_program("SunVS.glsl", "SunFS.glsl");
		GLuint skybox_program = load_program("SkyBoxVS.glsl", "SkyBoxFS.glsl");
		
		// Load all textures.
		GLuint skybox_tex = load_texture("./4k_stars_milky_way.png");
		
		// Create skybox geometry.
		GLuint skybox_vao, skybox_vbo, skybox_ibo;
		constexpr u32 skybox_long = 32;
		constexpr u32 skybox_lat = 16;
		constexpr u32 skybox_vert_count = skybox_long * (skybox_lat - 2) + 2;
		constexpr f32 skybox_radius = 10.0f;
		constexpr u32 skybox_idx_count = 6 * skybox_long * (skybox_lat - 2);
		{
			SkyBoxVertex* skybox_geo = (SkyBoxVertex*)malloc(skybox_vert_count * sizeof(SkyBoxVertex));
			skybox_geo[0].pos = Float3U(0.0f, -skybox_radius, 0.0f);
			for (u32 i = 1; i < skybox_lat - 1; ++i)
			{
				for (u32 j = 0; j < skybox_long; ++j)
				{
					f32 theta = lerp(0.0f, pi, (f32)i / (f32)(skybox_lat - 1));
					f32 theta2 = lerp(0.0f, 2 * pi, (f32)j / (f32)(skybox_long));
					skybox_geo[skybox_long * (i - 1) + 1 + j].pos = Float3U(
						sinf(theta) * skybox_radius * cosf(theta2),
						-cosf(theta) * skybox_radius,
						sinf(theta) * skybox_radius * sinf(theta2)
					);
				}
			}
			skybox_geo[skybox_vert_count - 1].pos = Float3U(0.0f, skybox_radius, 0.0f);
			u32* skybox_indices = (u32*)malloc(skybox_idx_count * sizeof(u32));
			u32 idx = 0;
			for (u32 i = 0; i < skybox_long; ++i)
			{
				skybox_indices[idx] = 0; ++idx;
				skybox_indices[idx] = i + 1; ++idx;
				skybox_indices[idx] = i + 2; ++idx;
			}
			for (u32 i = 0; i < skybox_lat - 3; ++i)
			{
				for (u32 j = 0; j < skybox_long; ++j)
				{
					skybox_indices[idx]	 = 1 + skybox_long * i + j; ++idx;
					skybox_indices[idx] = 1 + skybox_long * (i + 1) + j; ++idx;
					skybox_indices[idx] = 2 + skybox_long * i + j; ++idx;
					skybox_indices[idx] = 2 + skybox_long * i + j; ++idx;
					skybox_indices[idx] = 1 + skybox_long * (i + 1) + j; ++idx;
					skybox_indices[idx] = 2 + skybox_long * (i + 1) + j; ++idx;
				}
			}
			u32 base_index = 1 + skybox_long * (skybox_lat - 3);
			for (u32 i = 0; i < skybox_long; ++i)
			{
				skybox_indices[idx] = base_index + i; ++idx;
				skybox_indices[idx] = skybox_vert_count - 1; ++idx;
				skybox_indices[idx] = base_index + i + 1; ++idx;
			}
			luassert(idx == skybox_idx_count);

			glGenVertexArrays(1, &skybox_vao);
			glGenBuffers(1, &skybox_vbo);
			glGenBuffers(1, &skybox_ibo);
			glBindVertexArray(skybox_vao);
			glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
			glBufferData(GL_ARRAY_BUFFER, skybox_vert_count * sizeof(SkyBoxVertex), skybox_geo, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, skybox_idx_count * sizeof(u32), skybox_indices, GL_STATIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
			glEnableVertexAttribArray(0);

			free(skybox_geo);
			free(skybox_indices);
		}

		/*

		GLuint VAO, VBO, IBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void*)(3 * sizeof(f32)));
		glEnableVertexAttribArray(1);

		camera.transform.position = Float3(0.0f, 0.0f, -5.0f);*/

		bool mouse_pressed = false;
		Float2 mouse_click_pos;

		while (!glfwWindowShouldClose(window))
		{
			{
				constexpr f32 camera_speed = 0.1f;
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
				else
				{
					mouse_pressed = false;
				}
			}

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			Float4x4 model = Float4x4::make_affine_position_rotation_scale(camera.transform.position, Quaternion::identity(), Float3::one());
			Float4x4 view = camera.view_matrix();
			Float4x4 proj = camera.projection_matrix();

			// Draw Skybox.
			glDisable(GL_DEPTH_TEST);
			glUseProgram(skybox_program);
			glUniformMatrix4fv(glGetUniformLocation(skybox_program, "model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(skybox_program, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(skybox_program, "projection"), 1, GL_FALSE, &proj[0][0]);
			glUniform1i(glGetUniformLocation(skybox_program, "skybox_tex"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, skybox_tex);

			glBindVertexArray(skybox_vao);
			glDrawElements(GL_TRIANGLES, skybox_idx_count, GL_UNSIGNED_INT, NULL);

			glClearDepth(1.0f);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			glFlush();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glDeleteVertexArrays(1, &skybox_vao);
		glDeleteBuffers(1, &skybox_vbo);
		glDeleteBuffers(1, &skybox_ibo);
		glDeleteTextures(1, &skybox_tex);
		glDeleteProgram(skybox_program);

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