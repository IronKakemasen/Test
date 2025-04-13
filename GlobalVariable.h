#pragma once


namespace Torima
{
	float constexpr kPi = 3.141592653589f;
	float constexpr windowWidth = 1280.0f;
	float constexpr windowHeight = 720.0f;
	float constexpr torimaFOVy = 90.0f * kPi / 180.0f;
	float constexpr torimaFOVx = 121.0f * kPi / 180.0f;

	enum Surface
	{
		kFrontSide,
		kBackSide
	};

}