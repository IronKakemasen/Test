#pragma once
#include "BaseClass.h"

class Camera
{
public:

	Transform trans;
	RectShape rectShape;

public:

	//Constructor
	Camera() {};

	Camera(Vec4<float> pos_)
	{
		trans.pos = pos_;
		trans.rotateTheta.x = 180.0f / 12.0f;
	}

	//Destructor
	~Camera() {};

	Mat4 Get_MyMat() { return trans.mat; };
	Vec4<float> Get_MyPos() { return trans.pos; };

};

