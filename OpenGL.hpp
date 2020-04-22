#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"

#include "ECS.hpp"
#include "AssetManager.hpp"
#include "Input.hpp"

#include "glModel.hpp"
#include "glShader.hpp"
#include "glCamera.hpp"
#include "glCubemap.hpp"
#include "Phy7ics.hpp"
#include "Transformation.hpp"

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

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe

			glfwSwapInterval(1); // VSYNC

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
					myClass->MouseCallback(window, (float)xPos, (float)yPos);
				}
			);

			CheckExtension("GL_ARB_gpu_shader5");

			// Things that will later be removed from here when i start actually using ECS : 
			skybox = new Shader("skybox");
			//modelShader = new Shader("lighting");
			//modelShader = new Shader("lighting_array");
			modelShader = new Shader("textured");
			normalShader = new Shader("normal_b", "normal_b");
			lightShader = new Shader("light");
			//kyTexture = new CubemapTex("white");
			skyTexture = new CubemapTex("frozen");
			//skyTexture = new CubemapTex("detail");

			//defaultTexture = LoadTexture("detail/textures/default2.png", true);

			std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;

			//mdl = new Model("de_inferno");
			//mdl = new Model("nanosuit");
			//lamp = new Model("bulb");
			//mdl = new Model("de_inferno");

			return true;
		}

		void CheckExtension(std::string extensionName)
		{
			if (!glfwExtensionSupported(extensionName.c_str()))
			{
				std::cout << extensionName + " is not supported " << std::endl;
			}
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

		void Update(EntityController* entityController, AssetManager* assetManager, double currentTime, double deltaTime)
		{
			// Setting up the ECS stuff
			std::vector<Entity> entities = entityController->GetAllEntities();
			std::vector<Asset> assets = assetManager->GetAllAssets();
			
			playerCamera.ProcessKeyboardInput(input, (float)deltaTime);
			
			glfwPollEvents();
			
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glClearColor(0.1f, 0.1f, 0.8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glm::mat4 projection = glm::perspective(playerCamera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.01f, 1000.0f);
			glm::mat4 view = glm::mat4(1);
			glm::mat4 model = glm::mat4(1);
			
			view = glm::mat4(glm::mat3(playerCamera.GetViewMatrix()));
			skyTexture->Draw(*skybox, view, projection);
			
			view = playerCamera.GetViewMatrix();


			Transformation transform;

			for (Entity& entity : entities)
			{
				if (entity.components[CAT_DISABLED].GetType() != CAT_DISABLED)
				{
					if (entity.components[CAT_TRANSFORM].GetType() != CAT_DEFAULT)
					{
						Asset transformAsset = assetManager->GetAsset(entity.components[CAT_TRANSFORM].GetIndex());
						if (transformAsset.data.type() == typeid(Transformation))
						{
							transform = std::any_cast<Transformation>(transformAsset.data);
						}
						else
						{
							// error wtf ??
							pSendMessage(Message(MSG_LOG, std::string("OpenGL Error"), std::string("The Transformation component for entity '" + entity.name + "' is not of type Transformation.")));
						}
					}

					Asset asset = assetManager->GetAsset(entity.components[CAT_MODEL].GetIndex());

					model = glm::translate(model, glm::vec3(transform.translation.x, transform.translation.y, transform.translation.z));
					model = glm::scale(model, glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z));
					
					if (asset.assetType != CAT_DEFAULT)
					{
						if (asset.data.type() == typeid(Model))
						{
							Model mdl = std::any_cast<Model>(asset.data);
							if (!mdl.initialized)
							{
								Model procMdl = std::any_cast<Model>(asset.data);
								procMdl.InitOGLContext();
								asset.data = procMdl;
								assetManager->UpdateAsset(entity.components[CAT_MODEL].GetIndex(), asset);

								continue;
							}

							modelShader->Use();

							glUniformMatrix4fv(glGetUniformLocation(modelShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
							glUniformMatrix4fv(glGetUniformLocation(modelShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
							glUniformMatrix4fv(glGetUniformLocation(modelShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
							glUniform3f(glGetUniformLocation(modelShader->Program, "viewPos"), playerCamera.GetPosition().x, playerCamera.GetPosition().y, playerCamera.GetPosition().z);
							glUniform3f(glGetUniformLocation(modelShader->Program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

							mdl.Draw(modelShader);
						}
						else if (asset.data.type() == typeid(AABB))
						{
							AABB drawnAABB = std::any_cast<AABB>(asset.data);

							if (drawnAABB.VAO == 0 && drawnAABB.VBO == 0 && drawnAABB.EBO == 0)
							{
								glGenVertexArrays(1, &drawnAABB.VAO);
								glGenBuffers(1, &drawnAABB.VBO);
								glGenBuffers(1, &drawnAABB.EBO);
								glBindVertexArray(drawnAABB.VAO);
								glBindBuffer(GL_ARRAY_BUFFER, drawnAABB.VBO);
								glBufferData(GL_ARRAY_BUFFER, sizeof(drawnAABB.lines), drawnAABB.lines, GL_STATIC_DRAW);

								glEnableVertexAttribArray(0);
								glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
								glBindVertexArray(0);
							}
						}
					}
				}
			}

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
		//Model* mdl;
		Shader* modelShader;
		Shader* normalShader;
		Shader* lightShader;
		CubemapTex* skyTexture;
		vec3 lightPos = vec3(1.0f);
	};
}