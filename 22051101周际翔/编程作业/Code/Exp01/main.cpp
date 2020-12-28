#include <Runtime/Runtime.hpp>
#include <glad/gl.h>
#include <glfw-3.3.2/include/GLFW/glfw3.h>
#include <Runtime/Debug.hpp>
#include <Runtime/Math.hpp>
#include "Camera.hpp"

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

	GLint compile_shader()
	{
		const c8* vert = "  \
			#version 330 core\n\
			layout(location = 0) in vec3 aPos;\
			layout(location = 1) in vec3 aColor;\
			out vec3 Color;\
			uniform mat4 model;\
			uniform mat4 view;\
			uniform mat4 projection;\
			void main()\
			{\
				gl_Position = projection * view * model * vec4(aPos, 1.0f);\
				Color = aColor;\
			}";
		const c8* frag = "\
			#version 330 core\n\
			in vec3 Color;\
			out vec4 FragColor;\
			void main()\
			{\
				FragColor = vec4(Color.rgb, 1.0);\
			}";
		GLuint vertex, fragment;
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vert, NULL);
		glCompileShader(vertex);
		check_shader_compile_error(vertex, "VERTEX");
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &frag, NULL);
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

	struct Vertex
	{
		Float3U pos;
		Float3U color;
		Vertex(f32 x, f32 y, f32 z, f32 r, f32 g, f32 b) :
			pos(x, y, z),
			color(r, g, b) {}
	};

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

		glEnable(GL_DEPTH_TEST);

		GLuint program = compile_shader();

		Vertex vertices[6][4] = {
			// Left
			{
				Vertex(-0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.0f),
				Vertex(-0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f),
				Vertex(-0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f),
				Vertex(-0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.0f)
			},
			// Right
			{
				Vertex(0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f),
				Vertex(0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f),
				Vertex(0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f),
				Vertex(0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f)
			},
			// Down
			{
				Vertex(-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f),
				Vertex( 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f),
				Vertex( 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f),
				Vertex(-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f)
			},
			// Up
			{
				Vertex(-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f),
				Vertex( 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f),
				Vertex( 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f),
				Vertex(-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f)
			},
			// Back
			{
				Vertex(-0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f), // L D
				Vertex( 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f), // R D
				Vertex( 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f), // R U
				Vertex(-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f)  // L U
			},
			// Front
			{
				Vertex(-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f),
				Vertex( 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f),
				Vertex( 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f),
				Vertex(-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f)
			}
		};
		u32 indices[6][6];
		for (u32 i = 0; i < 6; ++i) {
			indices[i][0] = 0 + i * 4;
			indices[i][1] = 1 + i * 4;
			indices[i][2] = 2 + i * 4;
			indices[i][3] = 2 + i * 4;
			indices[i][4] = 3 + i * 4;
			indices[i][5] = 0 + i * 4;
		}

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

		camera.transform.position = Float3(0.0f, 0.0f, -5.0f);

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
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program);

			Float4x4 model = Float4x4::identity();
			Float4x4 view = camera.view_matrix();
			Float4x4 proj = camera.projection_matrix();

			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &proj[0][0]);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

			glFlush();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &IBO);
		glDeleteProgram(program);

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