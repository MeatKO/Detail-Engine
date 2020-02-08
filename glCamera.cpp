#include "glCamera.hpp"

namespace detailEngine
{
	Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
		: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
		: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM)
	{
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	glm::mat4 Camera::GetViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}

	void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->movementSpeed * deltaTime;

		if (direction == FORWARD)
		{
			this->position += this->front * velocity;
		}

		if (direction == BACKWARD)
		{
			this->position -= this->front * velocity;
		}

		if (direction == LEFT)
		{
			this->position -= this->right * velocity;
		}

		if (direction == RIGHT)
		{
			this->position += this->right * velocity;
		}
	}

	void Camera::ProcessKeyboardInput(Input* input, float deltaTime)
	{
		if (input->keyList[GLFW_KEY_W].isPressed)
		{
			ProcessKeyboard(FORWARD, deltaTime);
		}

		if (input->keyList[GLFW_KEY_S].isPressed)
		{
			ProcessKeyboard(BACKWARD, deltaTime);
		}

		if (input->keyList[GLFW_KEY_A].isPressed)
		{
			ProcessKeyboard(LEFT, deltaTime);
		}

		if (input->keyList[GLFW_KEY_D].isPressed)
		{
			ProcessKeyboard(RIGHT, deltaTime);
		}
	}

	void Camera::ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch)
	{
		xOffset *= this->mouseSensitivity;
		yOffset *= this->mouseSensitivity;

		this->yaw = glm::mod(this->yaw + xOffset, 360.0f);
		this->pitch += yOffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->pitch > 89.0f)
			{
				this->pitch = 89.0f;
			}

			if (this->pitch < -89.0f)
			{
				this->pitch = -89.0f;
			}
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();
	}

	void Camera::ProcessMouseScroll(GLfloat yOffset) { /* Will add zoom code... some day */ }

	glm::vec3 Camera::GetUp() { return this->up; }
	glm::vec3 Camera::GetPosition() { return this->position; }
	glm::vec3 Camera::GetFront() { return this->front; }
	glm::vec3 Camera::GetRight() { return this->right; }

	GLfloat Camera::GetYaw() { return this->yaw; }
	GLfloat Camera::GetPitch() { return this->pitch; }
	GLfloat Camera::GetZoom() { return this->zoom; }

	void Camera::SetY(GLfloat newY) { this->position.y = newY; }
	void Camera::PrintMatrix()
	{
		std::cout << glm::to_string(GetViewMatrix()) << std::endl;
	}
	void Camera::updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}
}

