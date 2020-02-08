#pragma once

//#define GLM_ENABLE_EXPERIMENTAL


#include <vector>
#include <iostream>
#include "GLM/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Input.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace detailEngine
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	const GLfloat YAW = -90.0f;
	const GLfloat PITCH = 0.0f;
	const GLfloat SPEED = 5.0f;
	const GLfloat SENSITIVTY = 0.1f;
	const GLfloat ZOOM = glm::radians(90.0f);

	class Camera
	{
	public:
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH);

		Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch);

		glm::mat4 GetViewMatrix();
		void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
		void ProcessKeyboardInput(Input* input, float deltaTime);
		void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true);
		void ProcessMouseScroll(GLfloat yOffset);

		glm::vec3 GetPosition();
		glm::vec3 GetUp();
		glm::vec3 GetFront();
		glm::vec3 GetRight();
		GLfloat GetYaw();
		GLfloat GetPitch();
		GLfloat GetZoom();
		void SetY(GLfloat newY);

		void PrintMatrix();

		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 worldUp;

		// We only need 2 angles to avoid gimbal locking
		GLfloat yaw;
		GLfloat pitch;

		GLfloat movementSpeed;
		GLfloat mouseSensitivity;
		GLfloat zoom;

		void updateCameraVectors();
	};
}