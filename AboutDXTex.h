#pragma once

#include "AboutWindow.h"
#include "External/DirectXTex/DirectXTex.h"
#include <map>

#include <vector>

struct TextureResourceData
{
	ID3D12Resource* textureResource;
	DirectX::ScratchImage mipImages;
	DirectX::TexMetadata metaData;
};

struct DirectXTextureSetUp
{
	std::vector<TextureResourceData> textureResourceDataList;
	int textureCount = 0;

	//テクスチャを読み込むための関数
	DirectX::ScratchImage LoadTexture(std::string const& filePath_)
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


	//読み込んだTextureの情報をもとにTextureResourceを作る関数
	ID3D12Resource* CreateTextureResource(ID3D12Device* device_, DirectX::TexMetadata& metaData_)
	{
		D3D12_RESOURCE_DESC resourceDesc{};

		//[ materialDataを元にResourceの設定 ]
		//textureの幅
		resourceDesc.Width = UINT(metaData_.width);
		//textureの高さ
		resourceDesc.Height = UINT(metaData_.height);
		//mipmapの数
		resourceDesc.MipLevels = UINT16(metaData_.mipLevels);
		//奥行orTextureの配列数
		resourceDesc.DepthOrArraySize = UINT16(metaData_.arraySize);
		//Textureのformat
		resourceDesc.Format = metaData_.format;
		//サンプリングカウント。１固定
		resourceDesc.SampleDesc.Count = 1;
		//テクスチャの次元数。2
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metaData_.dimension);

		//[ 利用するHeapの設定 ]
		//細かい設定を行う
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		//[ resourceを生成する ]
		ID3D12Resource* resource = nullptr;
		HRESULT hr = device_->CreateCommittedResource(
			&heapProperties,					//Heapの設定
			D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定、特になし
			&resourceDesc,						//Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST,		//データ転送される設定
			nullptr,							//Clear最適値。使わないのでnullptr
			IID_PPV_ARGS(&resource));			//作成するresourceへのポインタのポインタ

		assert(SUCCEEDED(hr));

		return resource;
	}

	
	TextureResourceData LoadTextureData(std::string filePath_, ID3D12Device* device_)
	{
		TextureResourceData ret_textureResourceData;
		ret_textureResourceData.mipImages = LoadTexture(filePath_);
		ret_textureResourceData.metaData = ret_textureResourceData.mipImages.GetMetadata();
		ret_textureResourceData.textureResource = CreateTextureResource(device_, ret_textureResourceData.metaData);

		++textureCount;

		////Textureを読み込んで転送する
		//DirectX::ScratchImage mipImages = LoadTexture(filePath_);
		//DirectX::TexMetadata metaData = mipImages.GetMetadata();
	
		return ret_textureResourceData;

	}

};




