#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"

#include "ECS.hpp"
#include "Input.hpp"

#include "glModel.hpp"
#include "glShader.hpp"
#include "glCamera.hpp"
#include "glCubemap.hpp"

namespace detailEngine
{
	class OpenGL : public Publisher, public Subscriber
	{
	public:
		OpenGL() {}

		double time = 0.0f;

		bool Init(std::string WindowName, int windowSizeX, int windowSizeY, Input* inputPtr)
		{
			input = inputPtr;
			if (!input)
			{
				pSendMessage(Message(MSG_LOG, std::string("OpenGL Error"), std::string("Input nullptr.")));
				return false;
			}
			WindowName = WindowName;

			if (!glfwInit())
				pSendMessage(Message(MSG_LOG, std::string("OpenGL Error"), std::string("Failed to initialize GLFW.")));

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
			glfwWindowHint(GLFW_SAMPLES, 4);                                          // MSAA

			// Fullscreen video mode settings
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			this->glWindow = glfwCreateWindow(windowSizeX, windowSizeY, WindowName.c_str(), nullptr, nullptr);

			if (!glWindow)
			{
				pSendMessage(Message(MSG_LOG, std::string("OpenGL Error"), std::string("Failed to create GLFW window.")));
				glfwTerminate();
				return false;
			}
			glfwGetFramebufferSize(glWindow, &SCREEN_WIDTH, &SCREEN_HEIGHT);
			glfwMakeContextCurrent(glWindow);
			//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glewExperimental = GL_TRUE;                                          // Enable modern GLEW

			if (glewInit() != GLEW_OK)
			{
				pSendMessage(Message(MSG_LOG, std::string("OpenGL Error"), std::string("Failed to initialize GLEW.")));
				glfwTerminate();
				return false;
			}

			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glCullFace(GL_BACK);
			glEnable(GL_MULTISAMPLE);
			glEnable(GL_STENCIL_TEST);
			glEnable(GL_FRAMEBUFFER_SRGB);

			if (glGenVertexArrays == NULL)
				pSendMessage(Message(MSG_LOG, std::string("OpenGL Error"), std::string("glGenVertexArray returned NULL at initialization.")));

			// Some lambda - pointer - stuff trickery below
			glfwSetWindowUserPointer(glWindow, this);

			glfwSetKeyCallback(
				glWindow,
				[](GLFWwindow* window, int key, int scancode, int action, int mode) -> void
				{
					OpenGL* myClass = (OpenGL*)glfwGetWindowUserPointer(window);
					myClass->KeyCallback(window, key, scancode, action, mode);
				}
			);

			glfwSetCursorPosCallback(
				glWindow,
				[](GLFWwindow* window, double xPos, double yPos) -> void
				{
					OpenGL* myClass = (OpenGL*)glfwGetWindowUserPointer(window);
					myClass->MouseCallback(window,(float)xPos, (float)yPos);
				}
			);

			skybox = new Shader("skybox");
			skyTexture = new CubemapTex("frozen");

			return true;
		}

		void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
		{
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(window, GL_TRUE);
			}

			if (key >= 0 && key < 1024)
			{
				if (action == GLFW_PRESS)
				{
					input->SetKey(key, time);
				}
				else if (action == GLFW_RELEASE)
				{
					input->UnsetKey(key, time);
				}
			}
		}

		void MouseCallback(GLFWwindow* window, float xPos, float yPos)
		{
			if (mouseInit)
			{
				mouseLastX = xPos;
				mouseLastY = yPos;

				mouseInit = false;
			}

			float xOffset = xPos - mouseLastX;
			float yOffset = mouseLastY - yPos;

			mouseLastX = xPos;
			mouseLastY = yPos;

			playerCamera.ProcessMouseMovement(xOffset, yOffset);

			//mouse->pSendMessage(Message(MSG_MOUSE, "MOVE"));
			//mouse->pSendMessage(KeyMessage("RELEASE", to_string(key)));
		}

		void Update(EntityController* entityController, double currentTime, double deltaTime)
		{
			glfwPollEvents();

			playerCamera.ProcessKeyboardInput(input, (float)deltaTime);

			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glClearColor(0.1f, 0.1f, 0.8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection = glm::perspective(playerCamera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
			glm::mat4 view = glm::mat4(glm::mat3(playerCamera.GetViewMatrix()));
			glm::mat4 model = glm::mat4();

			skyTexture->Draw(*skybox, view, projection);

			glfwSwapBuffers(glWindow);
		}

	private:
		Input* input = nullptr;
		int SCREEN_WIDTH = 1200, SCREEN_HEIGHT = 900;
		float mouseLastX = 0.0f, mouseLastY = 0.0f;
		bool mouseInit = true;
		GLFWwindow* glWindow = nullptr;
		Camera playerCamera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
		Shader* skybox;
		CubemapTex* skyTexture;
	};
}