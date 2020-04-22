#pragma once
#include "dMath.hpp"

/*
Position :
Absolute position = XYZ coordinates in space
Model Relative position = XYZ coordinates added to the target model's position
Camera Relative position = right * X, up * Y, front * Z multiplies of the camera vectors + the camera position

Rotation :
X, Y and Z define the angle of rotation along the axes

*/

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
		Transformation(TranslationType Type, std::string Model, dMath::vec3 Translation, dMath::vec3 Scale, dMath::vec3 Rotation) 
			: translationType(Type), targetModel(Model), translation(Translation), scale(Scale), rotation(Rotation) {}

		TranslationType translationType = TRA_ABSOLUTE;
		std::string targetModel = "";
		dMath::vec3 translation = dMath::vec3(0.0f);
		dMath::vec3 scale = dMath::vec3(1.0f);
		dMath::vec3 rotation = dMath::vec3(0.0f);
	};
}