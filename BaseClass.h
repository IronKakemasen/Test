#pragma once

#include "VecAndMat.h"
#include < vector >



class Transform
{

public:
	//行列	
	Mat4 mat;
	//ポず
	Vec4<float> pos;
	//バッファー
	std::vector<Vec4<float>> buff_pos;
	//スケール
	Vec4<float> scale = { 1.0f, 1.0f ,1.0f,1.0f };
	// 三次元回転用
	//回転（SRT）
	Vec4<float> rotateTheta = { 0.0f, 0.0f, 0.0f,0.0f };
	//回転（STR）
	Vec4<float> movementTheta = { 0.0f, 0.0f, 0.0f,0.0f };
	//方向
	Vec4<float> dir{ 1.0f, 1.0f ,1.0f ,1.0f };
	//速さ
	float deltaPos = 0;


	void Clear();
};

class TriangleShape
{

public:

	//ローカル頂点
	//TopVertex
	Vec4<float> local_Tv;
	//RightVertex
	Vec4<float> local_Rv;
	//LeftVertex
	Vec4<float> local_Lv;

	//頂点設定
	void SetVertex(Vec4<float> Tv_, Vec4<float> Rv_, Vec4<float> Lv_);

	//表か裏か
	static Torima::Surface GetSurfaceInfo(Vec4<float> Tv_, Vec4<float> Rv_, Vec4<float> Lv_, Vec4<float> cameraVec);

};

class RectShape
{

public:

	//ローカル頂点
	Vec4<float> LT;
	Vec4<float> RT;
	Vec4<float> LB;
	Vec4<float> RB;

	//頂点設定
	void SetVertex(float width_, float height_);
};

