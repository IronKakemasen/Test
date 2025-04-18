#pragma once
#include "VecAndMat.h"

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3(float entries[3]) : x(entries[0]), y(entries[1]),z(entries[2]) {}
	Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

};

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
	Vector3 normal;

};