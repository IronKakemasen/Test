#pragma once

#include "AboutWindow.h"
#include "AbouttDevice.h"
#include "AboutFence.h"
#include "AboutID3D12.h"
#include "AboutShaderCompile.h"
#include "AboutDXTex.h"


class MyDX
{

public:
	//ウィンドウ、デバッグ、例外設定？の初期化
	WindowSetUp windowSetUp;
	//デバイス
	DeviceSetUp deviceSetUp;
	//フェンス
	FenceSetUp fenceSetUp;
	//ID3D12?
	ID3D12SetUp iD3D12SetUp;
	//シェーダー
	ShaderCompileSetUp shaderSetUp;
	//TextureResource,mipData
	DirectXTextureSetUp dxTexSetUp;



	void Initialize();
	void Update();
	void Finalize();

};