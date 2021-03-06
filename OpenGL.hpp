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
#include "Log.hpp"

#include "glModel.hpp"
#include "glTexture.hpp"
#include "glShader.hpp"
#include "glCamera.hpp"
#include "glCubemap.hpp"
#include "Phy7ics.hpp"
#include "Transformation.hpp"
#include "PXL.hpp"
#include "VFS.hpp"
#include "WorldManager.hpp"

namespace detailEngine
{
	class Model;
	class Mesh;
	class Material;

	class OpenGL : public Publisher, public Subscriber
	{
	public:
		OpenGL();

		double time = 0.0f;

		bool Init(std::string WindowName, int windowSizeX, int windowSizeY, Input* inputPtr, int majorVersion, int minorVersion);

		void CheckExtension(std::string extensionName);

		void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

		void MouseCallback(GLFWwindow* window, float xPos, float yPos);

		void Update(EntityController* entityController, AssetManager* assetManager, WorldManager* worldManager, double currentTime, double deltaTime);

		void ProcessObjModel(Model& model);

		void DrawObj(Shader* shader, Model& model);

		void DrawMesh(Shader* shader, Mesh& mesh, Material& mat);

		int GenerateTexture(std::string& data, int width, int height);

		//int LoadTexture(std::string directory, bool nearest = true);

		float initPlaneVertices[20]
		{
			10.0f, 0.0f, 10.0f, 1.0f, 1.0f,
			10.0f, 0.0f, -10.0f, 1.0f, 0.0f,
			-10.0f, 0.0f, -10.0f, 0.0f, 0.0f,
			-10.0f, 0.0f, 10.0f, 0.0f, 1.0f
		};

		int initPlaceIndices[6]
		{
			0, 1, 2,
			2, 3, 0
		};

	private:
		std::mutex contextLock;

		Input* input = nullptr;
		int SCREEN_WIDTH = 1200, SCREEN_HEIGHT = 900;
		float mouseLastX = 0.0f, mouseLastY = 0.0f;
		bool mouseInit = true;
		GLFWwindow* glWindow = nullptr;

		Camera playerCamera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
		Shader* skybox;
		Shader* modelShader;
		Shader* normalShader;
		Shader* lightShader;
		CubemapTex* skyTexture;
		vec3 lightPos = vec3(1.0f);

		unsigned int defaultVAO, defaultVBO, defaultEBO;
		int defaultTextureID = 0;
	};
}