#pragma once

#include "External/DirectXTex/DirectXTex.h"
#include "AboutDebugLog.h"


struct DirectXTextureSetUp
{
	ID3D12Resource* textureResource;
	DirectX::TexMetadata metaData;
};

//テクスチャを読み込むための関数
inline DirectX::ScratchImage LoadTexture(std::string const& filePath_)
{
	//テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath_);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB, 0,
		mipImages);

	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;
}



