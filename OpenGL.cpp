#include "OpenGL.hpp"

namespace detailEngine
{
	OpenGL::OpenGL()
	{
	}

	bool OpenGL::Init(std::string WindowName, int windowSizeX, int windowSizeY, Input* inputPtr, int majorVersion, int minorVersion)
	{
		std::lock_guard<std::mutex> mut(contextLock);

		input = inputPtr;
		if (!input)
		{
			pSendMessage(Message(MSG_LOG, std::string("OpenGL Error"), std::string("Input nullptr.")));
			return false;
		}
		WindowName = WindowName;

		if (!glfwInit())
			pSendMessage(Message(MSG_LOG, std::string("OpenGL Error"), std::string("Failed to initialize GLFW.")));

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
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
			pSendMessage(Message(MSG_ERROR_MESSAGE, std::string("OpenGL Error"),
				std::string("Failed to create GLFW window. \n OpenGL Version" + std::to_string(majorVersion) + "." + std::to_string(minorVersion) + " Might not be supported.")));

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
			pSendMessage(Message(MSG_ERROR_MESSAGE, std::string("OpenGL Error"),
				std::string("Failed to Initialize GLEW. \n OpenGL Version" + std::to_string(majorVersion) + "." + std::to_string(minorVersion) + " Might not be supported.")));

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

		glGenVertexArrays(1, &defaultVAO);
		glGenBuffers(1, &defaultVBO);
		glBindVertexArray(defaultVAO);
		glBindBuffer(GL_ARRAY_BUFFER, defaultVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(initPlaneVertices), initPlaneVertices, GL_STATIC_DRAW);
		glGenBuffers(1, &defaultEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, defaultEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(initPlaceIndices), initPlaceIndices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);



		CheckExtension("GL_ARB_gpu_shader5");
		CheckExtension("GL_ARB_multi_draw_indirect");

		// Things that will later be removed from here when i start actually using ECS : 
		skybox = new Shader("skybox");
		modelShader = new Shader("textured");
		//modelShader = new Shader("lighting_array");
		normalShader = new Shader("normal_b", "normal_b");
		lightShader = new Shader("light");
		//skyTexture = new CubemapTex("detail");
		skyTexture = new CubemapTex("frozen");

		//defaultTextureID = LoadTexture("detail/textures/default.png");

		//dFile defaultTex;
		//Texture tex1;
		//std::string error;
		//
		//if (dLoadFile(defaultTex, "detail/textures/", "pepega", "bmp"))
		//{
		//	std::cout << "file loaded\n";
		//
		//	if (LoadTexture(tex1, defaultTex.fileName, defaultTex.fileType, defaultTex.bytes, defaultTex.byteSize, error))
		//	{
		//		std::cout << "texture initialized\n";
		//
		//		defaultTextureID = InitTexture(tex1);
		//	}
		//	else
		//	{
		//		std::cout << error << "\n";
		//	}
		//}

		std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;

		return true;
	}

	void OpenGL::CheckExtension(std::string extensionName)
	{
		if (!glfwExtensionSupported(extensionName.c_str()))
		{
			std::cout << extensionName + " is NOT supported " << std::endl;
		}
		else
		{
			std::cout << extensionName + " is supported " << std::endl;
		}
	}

	void OpenGL::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
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

	void OpenGL::MouseCallback(GLFWwindow* window, float xPos, float yPos)
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

		pSendMessage(Message(MSG_MOUSEDELTA, float(xOffset), float(yOffset)));
	}

	void OpenGL::Update(EntityController* entityController, AssetManager* assetManager, WorldManager* worldManager, double currentTime, double deltaTime)
	{
		//std::lock_guard<std::mutex> mut(contextLock);

		glfwPollEvents();

		playerCamera.ProcessKeyboardInput(input, (float)deltaTime);
			
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(0.1f, 0.1f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.01f, 1000.0f);
		glm::mat4 projection = glm::perspective(playerCamera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.01f, 1000.0f);
		glm::mat4 view = glm::mat4(1);
		glm::mat4 model = glm::mat4(1);
		
		view = glm::mat4(glm::mat3(playerCamera.GetViewMatrix()));
		skyTexture->Draw(*skybox, view, projection);
		
		view = playerCamera.GetViewMatrix();
		
		modelShader->Use();
		
		glUniformMatrix4fv(glGetUniformLocation(modelShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(modelShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(modelShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform3f(glGetUniformLocation(modelShader->Program, "viewPos"), playerCamera.GetPosition().x, playerCamera.GetPosition().y, playerCamera.GetPosition().z);
		glUniform3f(glGetUniformLocation(modelShader->Program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		
		glBindTexture(GL_TEXTURE_2D, defaultTextureID);
		
		glBindVertexArray(defaultVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, defaultEBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(glWindow);
	}

	void OpenGL::ProcessObjModel(Model& model)
	{
		std::lock_guard<std::mutex> mut(contextLock);
		// todo : use model2.hpp
		// write the function

		//std::cout << "OGL : processed model " << model.modelName << "\n";

		//model.init = true;

		for (Mesh& mesh : model.meshes)
		{
			std::cout << "mesh \n";
			glGenVertexArrays(1, &mesh.VAO);
			glGenBuffers(1, &mesh.VBO);
			glBindVertexArray(mesh.VAO);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
			glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);
			glGenBuffers(1, &mesh.EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

			// Vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

			// Vertex Normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

			// Vertex Texture Coordinates
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

			//// vertex tangent
			//glEnableVertexAttribArray(3);
			//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			//
			//// vertex bitangent
			//glEnableVertexAttribArray(4);
			//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
			//
			//// mesh ID
			//glEnableVertexAttribArray(5);
			//glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, meshID));

			glBindVertexArray(0);
		}
	}
	void OpenGL::DrawObj(Shader* shader, Model& model)
	{
		//for (Mesh& mesh : model.meshes)
		//{
		//	Material material;
		//	for (Material& mat : model.materials)
		//	{
		//		if (mesh.usedMaterial == mat.name)
		//		{
		//			material = mat;
		//		}
		//	}
		//
		//	DrawMesh(shader, mesh, material);
		//}
	}
	void OpenGL::DrawMesh(Shader* shader, Mesh& mesh, Material& mat)
	{
		//glActiveTexture(GL_TEXTURE0);
		//glUniform1i(glGetUniformLocation((shader->Program), "map_kd"), 0);
		//glBindTexture(GL_TEXTURE_2D, mat.map_kd_id);
		//
		//std::cout << mat.map_kd_id << "\n";
		//
		//glBindVertexArray(mesh.VAO);
		//glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
		//glBindVertexArray(0);
	}
	int OpenGL::GenerateTexture(std::string& data, int width, int height)
	{
		std::lock_guard<std::mutex> mut(contextLock);

		GLuint textureID;

		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);

	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_BYTE, data.c_str());
		//glGenerateMipmap(GL_TEXTURE_2D);
		//
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}

	//int OpenGL::LoadTexture(std::string directory, bool nearest)
	//{
	//	std::lock_guard<std::mutex> mut(contextLock);
	//
	//	GLuint textureID;
	//	int width, height;
	//
	//	glGenTextures(1, &textureID);
	//
	//	unsigned char* image = SOIL_load_image(directory.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	//
	//	glBindTexture(GL_TEXTURE_2D, textureID);
	//
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	if (nearest)
	//	{
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	}
	//	else
	//	{
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	}
	//
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//
	//	SOIL_free_image_data(image);
	//
	//	return textureID;
	//}
}
