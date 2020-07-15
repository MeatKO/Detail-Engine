#pragma once

/*
Position :
Absolute position = XYZ coordinates in space
Model Relative position = XYZ coordinates added to the target model's position
Camera Relative position = right * X, up * Y, front * Z multiplies of the camera vectors + the camera position

Rotation :
X, Y and Z define the angle of rotation along the axes

*/

using namespace glm;

namespace detailEngine
{
	enum TranslationType
	{
		TRA_ABSOLUTE,
		TRA_MODEL_RELATIVE,
		TRA_CAMERA_RELATIVE
	};

	class Transformation
	{
	public:
		Transformation() {}
		Transformation(TranslationType Type, std::string Model, vec3 Translation, vec3 Scale, vec3 Rotation) 
			: translationType(Type), targetModel(Model), translation(Translation), scale(Scale), rotation(Rotation) {}

		TranslationType translationType = TRA_ABSOLUTE;
		std::string targetModel = "";
		vec3 translation = vec3(0.0f);
		vec3 scale = vec3(1.0f);
		vec3 rotation = vec3(0.0f);
	};
}