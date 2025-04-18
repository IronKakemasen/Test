#pragma once
#include <math.h>
#include "GlobalVariable.h"


//□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□
//Matrix4x4　　　　 　　　　　　　　　　　　　　　　　　　　□
//□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□
class Matrix4
{

public:

	float m[4][4] =
	{
		{1.0f,0.0f,0.0f,0.0f},
		{0.0f,1.0f,0.0f,0.0f},
		{0.0f,0.0f,1.0f,0.0f},
		{0.0f,0.0f,0.0f,1.0f},
	};

	Matrix4 operator+=(Matrix4 other)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int k = 0; k < 4; ++k)
			{
				m[i][k] += other.m[i][k];
			}
		}

		return *this;
	}

	Matrix4 operator-=(Matrix4 other)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int k = 0; k < 4; ++k)
			{
				m[i][k] -= other.m[i][k];
			}
		}

		return *this;
	}




	//============Constructor==============
	Matrix4() {};

	Matrix4(float zero_)
	{
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
		zero_;
	};

	Matrix4(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9,
		float a10, float a11, float a12, float a13, float a14, float a15)
	{
		m[0][0] = a0; m[0][1] = a1; m[0][2] = a2; m[0][3] = a3;
		m[1][0] = a4; m[1][1] = a5; m[1][2] = a6; m[1][3] = a7;
		m[2][0] = a8; m[2][1] = a9; m[2][2] = a10; m[2][3] = a11;
		m[3][0] = a12; m[3][1] = a13; m[3][2] = a14; m[3][3] = a15;

	}
	//=====================================

	//▽▽▽▽▽▽▽▽▽▽▽▽▽▽マトリックス同士の積▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽
	Matrix4 Multiply(Matrix4 const& other)
	{
		Matrix4 ret_mat;

		ret_mat.m[0][0] = m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0] +
			m[0][2] * other.m[2][0] + m[0][3] * other.m[3][0];
		ret_mat.m[0][1] = m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1] +
			m[0][2] * other.m[2][1] + m[0][3] * other.m[3][1];
		ret_mat.m[0][2] = m[0][0] * other.m[0][2] + m[0][1] * other.m[1][2] +
			m[0][2] * other.m[2][2] + m[0][3] * other.m[3][2];
		ret_mat.m[0][3] = m[0][0] * other.m[0][3] + m[0][1] * other.m[1][3] +
			m[0][2] * other.m[2][3] + m[0][3] * other.m[3][3];

		ret_mat.m[1][0] = m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0] +
			m[1][2] * other.m[2][0] + m[1][3] * other.m[3][0];
		ret_mat.m[1][1] = m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1] +
			m[1][2] * other.m[2][1] + m[1][3] * other.m[3][1];
		ret_mat.m[1][2] = m[1][0] * other.m[0][2] + m[1][1] * other.m[1][2] +
			m[1][2] * other.m[2][2] + m[1][3] * other.m[3][2];
		ret_mat.m[1][3] = m[1][0] * other.m[0][3] + m[1][1] * other.m[1][3] +
			m[1][2] * other.m[2][3] + m[1][3] * other.m[3][3];

		ret_mat.m[2][0] = m[2][0] * other.m[0][0] + m[2][1] * other.m[1][0] +
			m[2][2] * other.m[2][0] + m[2][3] * other.m[3][0];
		ret_mat.m[2][1] = m[2][0] * other.m[0][1] + m[2][1] * other.m[1][1] +
			m[2][2] * other.m[2][1] + m[2][3] * other.m[3][1];
		ret_mat.m[2][2] = m[2][0] * other.m[0][2] + m[2][1] * other.m[1][2] +
			m[2][2] * other.m[2][2] + m[2][3] * other.m[3][2];
		ret_mat.m[2][3] = m[2][0] * other.m[0][3] + m[2][1] * other.m[1][3] +
			m[2][2] * other.m[2][3] + m[2][3] * other.m[3][3];

		ret_mat.m[3][0] = m[3][0] * other.m[0][0] + m[3][1] * other.m[1][0] +
			m[3][2] * other.m[2][0] + m[3][3] * other.m[3][0];
		ret_mat.m[3][1] = m[3][0] * other.m[0][1] + m[3][1] * other.m[1][1] +
			m[3][2] * other.m[2][1] + m[3][3] * other.m[3][1];
		ret_mat.m[3][2] = m[3][0] * other.m[0][2] + m[3][1] * other.m[1][2] +
			m[3][2] * other.m[2][2] + m[3][3] * other.m[3][2];
		ret_mat.m[3][3] = m[3][0] * other.m[0][3] + m[3][1] * other.m[1][3] +
			m[3][2] * other.m[2][3] + m[3][3] * other.m[3][3];

		return ret_mat;

	}//マトリックス同士の積

	//****************************************
	//https://suzulang.com/inverse-matrix-44/
	//****************************************
	//▽▽▽▽▽▽▽▽▽▽▽▽▽▽4x4行列の行列式を返す関数▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽
	float GetDeterminant4x4()
	{
		const float a11 = m[0][0];
		const float a12 = m[0][1];
		const float a13 = m[0][2];
		const float a14 = m[0][3];

		const float a21 = m[1][0];
		const float a22 = m[1][1];
		const float a23 = m[1][2];
		const float a24 = m[1][3];

		const float a31 = m[2][0];
		const float a32 = m[2][1];
		const float a33 = m[2][2];
		const float a34 = m[2][3];

		const float a41 = m[3][0];
		const float a42 = m[3][1];
		const float a43 = m[3][2];
		const float a44 = m[3][3];


		return a11 * a22 * a33 * a44 + a11 * a23 * a34 * a42 + a11 * a24 * a32 * a43
			+ a12 * a21 * a34 * a43 + a12 * a23 * a31 * a44 + a12 * a24 * a33 * a41
			+ a13 * a21 * a32 * a44 + a13 * a22 * a34 * a41 + a13 * a24 * a31 * a42
			+ a14 * a21 * a33 * a42 + a14 * a22 * a31 * a43 + a14 * a23 * a32 * a41
			- a11 * a22 * a34 * a43 - a11 * a23 * a32 * a44 - a11 * a24 * a33 * a42
			- a12 * a21 * a33 * a44 - a12 * a23 * a34 * a41 - a12 * a24 * a31 * a43
			- a13 * a21 * a34 * a42 - a13 * a22 * a31 * a44 - a13 * a24 * a32 * a41
			- a14 * a21 * a32 * a43 - a14 * a22 * a33 * a41 - a14 * a23 * a31 * a42;

	}//4x4行列の行列式を返す関数

	//▽▽▽▽▽▽▽▽▽▽▽▽▽▽4x4行列の逆行列を取得▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽
	Matrix4 GetInversed()
	{
		Matrix4 ret_mat;

		const float a11 = m[0][0];
		const float a12 = m[0][1];
		const float a13 = m[0][2];
		const float a14 = m[0][3];

		const float a21 = m[1][0];
		const float a22 = m[1][1];
		const float a23 = m[1][2];
		const float a24 = m[1][3];

		const float a31 = m[2][0];
		const float a32 = m[2][1];
		const float a33 = m[2][2];
		const float a34 = m[2][3];

		const float a41 = m[3][0];
		const float a42 = m[3][1];
		const float a43 = m[3][2];
		const float a44 = m[3][3];

		ret_mat.m[0][0] = a22 * a33 * a44 + a23 * a34 * a42 + a24 * a32 * a43 - a22 * a34 * a43 - a23 * a32 * a44 - a24 * a33 * a42;
		ret_mat.m[0][1] = a12 * a34 * a43 + a13 * a32 * a44 + a14 * a33 * a42 - a12 * a33 * a44 - a13 * a34 * a42 - a14 * a32 * a43;
		ret_mat.m[0][2] = a12 * a23 * a44 + a13 * a24 * a42 + a14 * a22 * a43 - a12 * a24 * a43 - a13 * a22 * a44 - a14 * a23 * a42;
		ret_mat.m[0][3] = a12 * a24 * a33 + a13 * a22 * a34 + a14 * a23 * a32 - a12 * a23 * a34 - a13 * a24 * a32 - a14 * a22 * a33;
		ret_mat.m[1][0] = a21 * a34 * a43 + a23 * a31 * a44 + a24 * a33 * a41 - a21 * a33 * a44 - a23 * a34 * a41 - a24 * a31 * a43;
		ret_mat.m[1][1] = a11 * a33 * a44 + a13 * a34 * a41 + a14 * a31 * a43 - a11 * a34 * a43 - a13 * a31 * a44 - a14 * a33 * a41;
		ret_mat.m[1][2] = a11 * a24 * a43 + a13 * a21 * a44 + a14 * a23 * a41 - a11 * a23 * a44 - a13 * a24 * a41 - a14 * a21 * a43;
		ret_mat.m[1][3] = a11 * a23 * a34 + a13 * a24 * a31 + a14 * a21 * a33 - a11 * a24 * a33 - a13 * a21 * a34 - a14 * a23 * a31;
		ret_mat.m[2][0] = a21 * a32 * a44 + a22 * a34 * a41 + a24 * a31 * a42 - a21 * a34 * a42 - a22 * a31 * a44 - a24 * a32 * a41;
		ret_mat.m[2][1] = a11 * a34 * a42 + a12 * a31 * a44 + a14 * a32 * a41 - a11 * a32 * a44 - a12 * a34 * a41 - a14 * a31 * a42;
		ret_mat.m[2][2] = a11 * a22 * a44 + a12 * a24 * a41 + a14 * a21 * a42 - a11 * a24 * a42 - a12 * a21 * a44 - a14 * a22 * a41;
		ret_mat.m[2][3] = a11 * a24 * a32 + a12 * a21 * a34 + a14 * a22 * a31 - a11 * a22 * a34 - a12 * a24 * a31 - a14 * a21 * a32;
		ret_mat.m[3][0] = a21 * a33 * a42 + a22 * a31 * a43 + a23 * a32 * a41 - a21 * a32 * a43 - a22 * a33 * a41 - a23 * a31 * a42;
		ret_mat.m[3][1] = a11 * a32 * a43 + a12 * a33 * a41 + a13 * a31 * a42 - a11 * a33 * a42 - a12 * a31 * a43 - a13 * a32 * a41;
		ret_mat.m[3][2] = a11 * a23 * a42 + a12 * a21 * a43 + a13 * a22 * a41 - a11 * a22 * a43 - a12 * a23 * a41 - a13 * a21 * a42;
		ret_mat.m[3][3] = a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a11 * a23 * a32 - a12 * a21 * a33 - a13 * a22 * a31;

		float det = GetDeterminant4x4();

		if (det != 0.0f)
		{
			float inv_det = 1.0f / det;

			for (int i = 0; i < 4; i++)
			{
				for (int k = 0; k < 4; k++)
				{
					ret_mat.m[i][k] *= inv_det;
				}
			}
		}

		return ret_mat;

	}//4x4行列の逆行列を取得

};

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3(float entries[3]) : x(entries[0]), y(entries[1]), z(entries[2]) {}
	Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

};

struct Vector2
{
	float u;
	float v;

	Vector2(float entries[2]) : u(entries[0]), v(entries[1]) {}
	Vector2(float u_, float v_) : u(u_), v(v_) {}

};



//□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□
//Vector4  　　　　 　　　　　　　　　　　　　　　　　　　　□
//□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□
template<typename T>
class Vector4
{

public:

	T x = 0;
	T y = 0;
	T z = 0;
	T w = 1;

	//float magnitude = 0.0f;

	//============Constractor==============
	Vector4() {};
	Vector4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
	Vector4(T entries[4]) : x(entries[0]), y(entries[1]), z(entries[2]), w(entries[3]) {}
	//=====================================

	Vector4<T> operator+=(T const addNum)
	{
		x += addNum;
		y += addNum;
		z += addNum;

		return *this;
	}

	Vector4<T> operator=(T const num)
	{
		x = num;
		y = num;
		z = num;

		return *this;
	}

	//大きさをセット
	float GetMagnitutde()
	{
		return sqrtf(x * x + y * y + z * z);
	}

	//正規化ベクトルを返す
	Vector4<T> GetNormalizedVec()
	{
		float mag = GetMagnitutde();
		float inv_mag = 0;

		if (mag != 0) inv_mag = 1.0f / mag;

		Vector4<T> ret_normalizeVec = { x * inv_mag ,y * inv_mag,z * inv_mag,1.0f };

		return ret_normalizeVec;
	}

	//内積を計算して返す
	float GetDotProductionResult(Vector4<T> src1_Vector4, Vector4<T> src2_Vector4)
	{
		float ret_cosTheta = 0.0f;

		//正規化する
		Vector4<T> normalized1 = src1_Vector4.GetNormalizedVec();
		Vector4<T> normalized2 = src2_Vector4.GetNormalizedVec();
		//cos(🍆角)
		ret_cosTheta =
			normalized1.x * normalized2.x +
			normalized1.y * normalized2.y +
			normalized1.z * normalized2.z;

		return ret_cosTheta;
	}

	//差分単位ベクトルを返す
	Vector4 GetNormalizedDiffVector4(Vector4<T> dst_pos, Vector4<T> my_pos)
	{
		Vector4<T> diff;

		//差分を計算
		diff = dst_pos - my_pos;
		//大きさを計算
		diff.SetMagnitutde();
		Vector4<T> normalized_diff = diff.GetNormalizedVec();

		return normalized_diff;
	}

	//マトリックスとベクタの積
	Vector4<T> GetMultipliedByMat(Matrix4 src_mat)
	{
		Vector4<T> ret_Vector4;

		ret_Vector4.x = x * src_mat.m[0][0] + y * src_mat.m[1][0] +
			z * src_mat.m[2][0] + w * src_mat.m[3][0];

		ret_Vector4.y = x * src_mat.m[0][1] + y * src_mat.m[1][1] +
			z * src_mat.m[2][1] + w * src_mat.m[3][1];

		ret_Vector4.z = x * src_mat.m[0][2] + y * src_mat.m[1][2] +
			z * src_mat.m[2][2] + w * src_mat.m[3][2];

		ret_Vector4.w = x * src_mat.m[0][3] + y * src_mat.m[1][3] +
			z * src_mat.m[2][3] + w * src_mat.m[3][3];

		return ret_Vector4;
	}

	//クロス積を返す
	Vector4<float> GetCross(Vector4<float> other)
	{
		Vector4<T> ret_Vector4;

		ret_Vector4.x = y * other.z - z * other.y;
		ret_Vector4.y = z * other.x - x * other.z;
		ret_Vector4.z = x * other.y - y * other.x;

		return ret_Vector4;
	}

};

//==================================Operand============================================================
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓Operand↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
template<typename T>
Vector4<T> operator+(Vector4<T> const& lVec, Vector4<T> const& rVec2)
{
	Vector4<T> ret_vec = { lVec.x + rVec2.x,lVec.y + rVec2.y ,lVec.z + rVec2.z ,1 };

	return ret_vec;
}

template<typename T>
Vector4<T> operator-(Vector4<T> const& lVec, Vector4<T> const& rVec2)
{
	Vector4<T> ret_vec = { lVec.x - rVec2.x,lVec.y - rVec2.y ,lVec.z - rVec2.z ,1 };

	return ret_vec;
}

template<typename T>
Vector4<T> operator*(Vector4<T> const& dst_vec, T const multipleNum)
{
	Vector4<T> ret_vec = { dst_vec.x * multipleNum,dst_vec.y * multipleNum ,dst_vec.z * multipleNum ,1 };

	return ret_vec;
}

inline Matrix4 operator*(Matrix4 const& dst_mat, float multipleNum)
{
	Matrix4 ret_mat;

	for (int i = 0; i < 4; ++i)
	{
		for (int k = 0; k < 4; ++k)
		{
			ret_mat.m[i][k] = dst_mat.m[i][k] * multipleNum;
		}
	}

	return ret_mat;
}


//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑Operand↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
//==================================Operand============================================================


//▽▽▽▽▽▽▽▽▽▽▽▽▽▽SRT行列の作成・取得▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽
inline Matrix4 Get_SRTMat3D(const Vector4<float>& scale_, const Vector4<float>& rotateTheta_,
	const Vector4<float>& translate_)
{
	Matrix4 ret_mat;

	//回転角をradianに変換
	static float const degreeConverter = 3.141592653589f / 180.0f;
	Vector4<float> rotateRad = rotateTheta_ * degreeConverter;

	//3つの回転軸に対応した行列を作成
	//角度を三角関数に代入して返すラムダ式
	auto substituteRad = [](float rotateRad_) {
		Vector4<float> ret_vec = { cosf(rotateRad_),sinf(rotateRad_),1.0f,1.0f };
		return ret_vec; };

	//x軸
	Vector4<float> circularFunc = substituteRad(rotateRad.x);

	Matrix4 rotateMatX(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, circularFunc.x, circularFunc.y, 0.0f,
		0.0f, -circularFunc.y, circularFunc.x, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	//y軸
	circularFunc = substituteRad(rotateRad.y);
	Matrix4 rotateMatY =
	{
		circularFunc.x,0.0f,-circularFunc.y,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		circularFunc.y,0.0f,circularFunc.x,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	//z軸
	circularFunc = substituteRad(rotateRad.z);
	Matrix4 rotateMatZ =
	{
		circularFunc.x,circularFunc.y,0.0f,0.0f,
		-circularFunc.y,circularFunc.x,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	//3つの回転マトリックスを合成する
	Matrix4 concentratedRotateMat = rotateMatX.Multiply(rotateMatY.Multiply(rotateMatZ));

	//SRT行列の作成
	//ScaleMatrix
	Matrix4 scaleMat =
	{
		scale_.x,0.0f,0.0f,0.0f,
		0.0f,scale_.y,0.0f,0.0f,
		0.0f,0.0f,scale_.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	//TranslateMat
	Matrix4 translateMat =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		translate_.x,translate_.y,translate_.z,1.0f
	};

	//S⇔R
	ret_mat = scaleMat.Multiply(concentratedRotateMat);
	//SR⇔T
	ret_mat = ret_mat.Multiply(translateMat);

	return ret_mat;		//SRT

}//SRT行列の作成・取得

//▽▽▽▽▽▽▽▽▽▽▽▽▽▽STR行列の作成・取得▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽
inline Matrix4 Get_STRMat3D(const Vector4<float>& scale_, const Vector4<float>& movementTheta_,
	const Vector4<float>& translate_)
{
	Matrix4 ret_mat;

	//回転角をradianに変換
	static float const degreeConverter = Torima::kPi / 180.0f;
	Vector4<float> rotateRad = movementTheta_ * degreeConverter;

	//3つの回転軸に対応した行列を作成
	//角度を三角関数に代入して返すラムダ式
	auto substituteRad = [](float movementTheta_) {
		Vector4<float> ret_vec = { cosf(movementTheta_),sinf(movementTheta_),1.0f,1.0f };
		return ret_vec; };

	//x軸
	Vector4<float> circularFunc = substituteRad(rotateRad.x);
	Matrix4 movementMatX =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,circularFunc.x,circularFunc.y,0.0f,
		0.0f,-circularFunc.y,circularFunc.x,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	//y軸
	circularFunc = substituteRad(rotateRad.y);
	Matrix4 movementMatY =
	{
		circularFunc.x,0.0f,-circularFunc.y,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		circularFunc.y,0.0f,circularFunc.x,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	//z軸
	circularFunc = substituteRad(rotateRad.z);
	Matrix4 movementMatZ =
	{
		circularFunc.x,circularFunc.y,0.0f,0.0f,
		-circularFunc.y,circularFunc.x,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	//3つの回転マトリックスを合成する
	Matrix4 concentratedRotateMat = movementMatX.Multiply(movementMatY.Multiply(movementMatZ));

	//SRT行列の作成
	//ScaleMatrix
	Matrix4 scaleMat =
	{
		scale_.x,0.0f,0.0f,0.0f,
		0.0f,scale_.y,0.0f,0.0f,
		0.0f,0.0f,scale_.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	//TranslateMat
	Matrix4 translateMat =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		translate_.x,translate_.y,translate_.z,1.0f
	};

	//S⇔T
	ret_mat = scaleMat.Multiply(translateMat);
	//ST⇔R
	ret_mat = ret_mat.Multiply(concentratedRotateMat);

	return ret_mat;		//STR

}//STR行列の作成・取得


//正射影行列3D
inline Matrix4 Get_Orthographic3D(
	const float l_, const float r_,
	const float t_, const float b_,
	const float zn_, const float zf_)
{
	const float inv_W{ 1.0f / (r_ - l_) };
	const float inv_H{ 1.0f / (t_ - b_) };
	const float inv_D{ 1.0f / (zf_ - zn_) };

	return Matrix4
	{
		2.0f * inv_W, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f * inv_H, 0.0f, 0.0f,
		0.0f, 0.0f, inv_D, 0.0f,
		-(l_ + r_) * inv_W, -(t_ + b_) * inv_H, -zn_ * inv_D, 1.0f,
	};
}

//ビューポート変換3D
static inline Matrix4 Get_ViewportTransformation3D(
	const float left_ = 0.0f, const float top_ = 0.0f,
	const float windowWidth_ = Torima::windowWidth, const float windowHeight_ = Torima::windowHeight,
	const float minDepth_ = 0.0f, const float maxDepth_ = 1.0f)
{
	return Matrix4
	{
		windowWidth_ * 0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -windowHeight_ * 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, maxDepth_ - minDepth_, 0.0f,
		left_ + windowWidth_ * 0.5f, top_ + windowHeight_ * 0.5f, minDepth_, 1.0f,
	};
}

//透視投影行列
inline Matrix4 Get_PerspectiveFOV(float fovY_, float aspectRatio_, float nearClip_ = 0.1f, float farClip_ = 1000.0f)
{
	const float cotTheta{ 1.0f / tanf(fovY_ * 0.5f) };
	const float inv_frustumHeight{ 1.0f / (farClip_ - nearClip_) };

	return Matrix4
	{
		cotTheta / aspectRatio_ , 0.0f, 0.0f, 0.0f,
		0.0f, cotTheta, 0.0f, 0.0f,
		0.0f, 0.0f, farClip_ * inv_frustumHeight, 1.0f,
		0.0f, 0.0f, -nearClip_ * farClip_ * inv_frustumHeight, 0.0f,
	};

	//return Matrix4
	//{
	//	cotTheta / aspectRatio_ , 0.0f, 0.0f, 0.0f,
	//	0.0f, cotTheta, 0.0f, 0.0f,
	//	0.0f, 0.0f, -farClip_ * inv_frustumHeight,(-farClip_ * inv_frustumHeight) * nearClip_,
	//	0.0f, 0.0f, -1.0f, 0.0f,
	//};

}

//VP行列
inline Matrix4 Get_VPMat([[maybe_unused]] Vector4<float> cameraPos, Matrix4 cameraMat)
{
	Matrix4 ret_mat;

	//ビュー行列作成
	Matrix4 viewMat = cameraMat.GetInversed();

	//射影行列作成
	Matrix4 projectionMat;

	////平行投影
	//projectionMat = Get_Orthographic3D(
	//	cameraPos.x - (Torima::windowWidth * 0.5f), cameraPos.x + (Torima::windowWidth * 0.5f),
	//	cameraPos.y + (Torima::windowHeight * 0.5f), cameraPos.y - (Torima::windowHeight * 0.5f),
	//	0.1f, 1000.0f);

	//中心投影
	float aspectRatio = Torima::windowWidth / Torima::windowHeight;
	projectionMat = Get_PerspectiveFOV(Torima::torimaFOVy, aspectRatio);

	//viewMat⇔PerspectiveMat
	ret_mat = viewMat.Multiply(projectionMat);

	return  ret_mat;
}

