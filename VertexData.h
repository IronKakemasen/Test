#pragma once
#include "VecAndMat.h"

struct Vector2
{
	float u;
	float v;

	Vector2(float entries[2]) : u(entries[0]), v(entries[1]) {}
	Vector2(float u_, float v_) : u(u_), v(v_) {}

};

struct VertexData
{
	Vector4<float> position;
	Vector2 texcoord;

};