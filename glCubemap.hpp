#pragma once

#include <vector>
#include <GL/glew.h>

#include "Log.hpp"
#include "GLM/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SOIL2/SOIL2.h"
#include "glShader.hpp"

namespace detailEngine
{
	class CubemapEnv
	{
	public:
		CubemapEnv(glm::vec3 cameraPosition, unsigned int WIDTH, unsigned int HEIGHT);

		unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;
		unsigned int cubemapFBO;
		unsigned int cubemapTextureId;

		const float cubemap_near = 0.01f;
		const float cubemap_far = 30.0f;

		glm::mat4 viewProj;
		glm::vec3 position;
		std::vector<glm::mat4> viewTransforms;

		void Init();
		void Start();
		void End();
		void Update();
	};

	class CubemapTex
	{
	public:

		CubemapTex(std::string path);

		void Draw(Shader skyboxShader, glm::mat4 view, glm::mat4 projection);
		unsigned int loadCubemap(std::vector<std::string> faces);

		unsigned int skyboxVAO, skyboxVBO, cubemapTextureId;
	};
}