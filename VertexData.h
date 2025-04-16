#pragma once
#include "VecAndMat.h"

struct Vec2
{
	float u;
	float v;

	Vec2(float entries[2]) : u(entries[0]), v(entries[1]) {}
	Vec2(float u_, float v_) : u(u_), v(v_) {}

};

struct VertexData
{
	Vec4<float> position;
	Vec2 texcoord;

};