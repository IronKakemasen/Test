#pragma once
#include "VecAndMat.h"

struct DirectionalLight
{
	Vector4<float> color;
	Vector3 direction;
	float intensity;
};