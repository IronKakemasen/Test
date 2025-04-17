#pragma once

#include "VecAndMat.h"
#include < vector >



class Transform
{

public:
	//行列	
	Matrix4 mat;
	//ポず
	Vector4<float> pos;
	//バッファー
	std::vector<Vector4<float>> buff_pos;
	//スケール
	Vector4<float> scale = { 1.0f, 1.0f ,1.0f,1.0f };
	// 三次元回転用
	//回転（SRT）
	Vector4<float> rotateTheta = { 0.0f, 0.0f, 0.0f,0.0f };
	//回転（STR）
	Vector4<float> movementTheta = { 0.0f, 0.0f, 0.0f,0.0f };
	//方向
	Vector4<float> dir{ 1.0f, 1.0f ,1.0f ,1.0f };
	//速さ
	float deltaPos = 0;


	void Clear();
};

class TriangleShape
{

public:

	//ローカル頂点
	//TopVertex
	Vector4<float> local_Tv;
	//RightVertex
	Vector4<float> local_Rv;
	//LeftVertex
	Vector4<float> local_Lv;

	//頂点設定
	void SetVertex(Vector4<float> Tv_, Vector4<float> Rv_, Vector4<float> Lv_);

	//表か裏か
	static Torima::Surface GetSurfaceInfo(Vector4<float> Tv_, Vector4<float> Rv_, Vector4<float> Lv_, Vector4<float> cameraVec);

};

class RectShape
{

public:

	//ローカル頂点
	Vector4<float> LT;
	Vector4<float> RT;
	Vector4<float> LB;
	Vector4<float> RB;

	//頂点設定
	void SetVertex(float width_, float height_);
};

