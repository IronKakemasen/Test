#pragma once

#include "AboutWindow.h"
#include <dxcapi.h>
#include "VecAndMat.h"
#include "External/DirectXTex/DirectXTex.h"


struct ID3D12SetUp
{
	//コマンドキュー
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	
	//コマンドアローケータ
	ID3D12CommandAllocator* commandAllocator = nullptr;
	
	//コマンドリスト		
	ID3D12GraphicsCommandList* commandList = nullptr;
	
	//スワップチェーン
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//ReadTargetViewのディスクリプタヒープ
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	
	//ShadeResourceViewのディスクリプタヒープ
	ID3D12DescriptorHeap* srvDescriptorHeap = nullptr;

	//swapchainからのリソースを入れる箱
	ID3D12Resource* swapChainResources[2] = { nullptr };
	
	//RTV?
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//RTVを2つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSIgnature{};
	//具体的にshaderがどこかでデータを読めばいいのかの情報をまとめたもの
	//shaderとPSOのバインダー
	ID3D12RootSignature* rootSignature = nullptr;
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	//複数設定できるので配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//Shaderで扱うViewのDescriptorを設定するために使う
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	//BlendState
	D3D12_BLEND_DESC blendDesc{};

	//RasiterzerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graghicsPipeLineStatedesc{};
	ID3D12PipelineState* graghicsPipelineState = nullptr;

	//VertexResource
	ID3D12Resource* vertexResource = nullptr;
	//MaterialResource
	ID3D12Resource* materialResource = nullptr;
	//WVP用のリソース(Matrix4x4 1つ分のサイズ)
	ID3D12Resource* wvpResource;
	//wvp行列のアドレス
	Mat4* wvpData = nullptr;

	//VBV
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//ViewPortMat
	D3D12_VIEWPORT viewPort{};
	//シザー矩形
	D3D12_RECT scissorRect{};
	
	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

	//textureHandle
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = {};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = {};

	D3D12_RESOURCE_DESC resourceDesc{};

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	//DepthStencilTextureResource
	ID3D12Resource* depthStencilTextureResource;
	ID3D12DescriptorHeap* dsvDescriptorHeap;
	D3D12_CLEAR_VALUE depthClearValue{};
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;



	Vec4<float>* materialData = nullptr;
	VertexData* vertexData = nullptr;

	//DescriptorRangeの設定を行う
	void SetShaderViewDescriptorRange ()
	{
		//0から始まる
		descriptorRange[0].BaseShaderRegister = 0;
		//数は1つ
		descriptorRange[0].NumDescriptors = 1;
		//SRVを使う
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		//offsetを自動計算
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}


	//textureデータを転送する関数
	void UploadtextureData(ID3D12Resource* texture_, DirectX::ScratchImage const& mipImages)
	{
		//Meta情報を取得
		DirectX::TexMetadata const& metaData = mipImages.GetMetadata();
		//全MipMapについて
		for (size_t mipLevel = 0; mipLevel < metaData.mipLevels; ++mipLevel)
		{
			//MipMapLevelを指定して各Imageを取得
			DirectX::Image const* img = mipImages.GetImage(mipLevel, 0, 0);
			HRESULT hr = texture_->WriteToSubresource(
				UINT(mipLevel),			
				nullptr,				//全領域へコピー
				img->pixels,			//元データアドレス
				UINT(img->rowPitch),	//１ラインサイズ
				UINT(img->slicePitch)	//１枚サイズ
			);

			assert(SUCCEEDED(hr));

		}
	}

	//DepthStencilViewDesc(DSV)の設定
	void SetDepthStencilViewDesc(ID3D12Device* device_)
	{
		//format。基本的にはリソースに合わせる
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//2DTexture
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		//DSVHeapの先頭にDSVを作る
		device_->CreateDepthStencilView(depthStencilTextureResource, &dsvDesc,
			dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	}

	//DepthStencilを生成する
	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device_, int32_t width_, int32_t height_)
	{
		//生成するResourceの設定
		//テクスチャのサイズ
		resourceDesc.Width = width_;
		resourceDesc.Height = height_;
		//mipmapの数
		resourceDesc.MipLevels = 1;
		//奥行or配列Textureの配列数
		resourceDesc.DepthOrArraySize = 1;
		//depthstencilとして利用可能なフォーマット
		resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//サンプリングカウント1固定
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		//DepthStencilとして使う通知
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		//利用するHeapの設定
		//VRAM上に作る
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		//深度値クリア設定
		//最大値（1.0）でクリア
		depthClearValue.DepthStencil.Depth = 1.0f;
		//フォーマット。resourceと合わせる
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		//Resourceの生成
		ID3D12Resource* ret_resource = nullptr;
		HRESULT hr = device_->CreateCommittedResource
		(
			&heapProperties,					//Heapの設定
			D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定：特になし
			&resourceDesc,						//resourceの設定
			D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値を書き込む状態にしておく
			&depthClearValue,					//Clear最適値
			IID_PPV_ARGS(&ret_resource)			//作成するリソースポインタへのポインタ
		);

		assert(SUCCEEDED(hr));

		return ret_resource;
	}

	//読み込んだTextureの情報をもとにTextureResourceを作る関数
	ID3D12Resource* CreateTextureResource(ID3D12Device* device_, DirectX::TexMetadata& metaData_)
	{
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
		heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
		//WriteBackポロシーでCPUアクセス可能
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		//プロセッサの近くに設置
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

		//[ resourceを生成する ]
		ID3D12Resource* resource = nullptr;
		HRESULT hr = device_->CreateCommittedResource(
			&heapProperties,					//Heapの設定
			D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定、特になし
			&resourceDesc,						//Resourceの設定
			D3D12_RESOURCE_STATE_GENERIC_READ,	//初回のResoourceState。textureは読むだけ
			nullptr,							//Clear最適値。使わないのでnullptr
			IID_PPV_ARGS(&resource));			//作成するresourceへのポインタのポインタ

		assert(SUCCEEDED(hr));

		return resource;
	}

	//DescriptorHeapの作成関数
	ID3D12DescriptorHeap* MakeDescriptorHeap(ID3D12Device* device_,
		D3D12_DESCRIPTOR_HEAP_TYPE heapType_, UINT numDescriptors_, bool shaderVisible)
	{
		ID3D12DescriptorHeap* ret_descriptorHeap = nullptr;
		D3D12_DESCRIPTOR_HEAP_DESC descriptorheapDesc{};
		descriptorheapDesc.Type = heapType_;
		descriptorheapDesc.NumDescriptors = numDescriptors_;
		descriptorheapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		
		HRESULT hr = device_->CreateDescriptorHeap(&descriptorheapDesc,
			IID_PPV_ARGS(&ret_descriptorHeap));

		assert(SUCCEEDED(hr));

		return ret_descriptorHeap;
	}

	//ConstantBufferを1つ読むための設定（関数内部）
	void SetRootPara()
	{
		//CBVを使う(b0のb)
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		//PixcelShaderで使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		//レジスタ番号0とバインド
		rootParameters[0].Descriptor.ShaderRegister = 0;

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		//VertexShaderで使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		//レジスタ番号0とバインド
		rootParameters[1].Descriptor.ShaderRegister = 0;

		//Descriptortableを使う
		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		//pixcelShaderを使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		//tableの中身の配列を指定
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
		//tableで利用する
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

		//ルートパラメータ配列へのポインタ
		descriptionRootSIgnature.pParameters = rootParameters;
		//配列の長さ
		descriptionRootSIgnature.NumParameters = _countof(rootParameters);
	}

	//DXの行列の設定
	void SetDXMatrix(int32_t kClientWidth_, int32_t kClientHeight_)
	{
		//クライアント領域と一緒のサイズにして画面全体に表示
		viewPort.Width = static_cast<FLOAT>(kClientWidth_);
		viewPort.Height = static_cast<FLOAT>(kClientHeight_);
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		//しざー矩形
		scissorRect.right = static_cast<LONG>(kClientWidth_);
		scissorRect.bottom = static_cast<LONG>(kClientHeight_);
		scissorRect.left = 0;
		scissorRect.top = 0;
	}

	//wvpリソースにデータを書き込む
	void OverrideWVPData()
	{
		wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
		//単位行列を書き込んでおく
		Mat4 tmp;
		*wvpData = tmp;
	}


	//マテリアルリソースにデータを書き込む
	void OverrideMaterialData()
	{
		//書き込むためのアドレスを取得
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
		//色
		*materialData = { 1.0f,1.0f,1.0f,1.0f };
	}

	//頂点リソースにデータを書き込む
	void OverrideVertexData()
	{
		//書き込むためのアドレスを取得
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
		//左下
		vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
		vertexData[0].texcoord = { 0.0f,1.0f };
		//上
		vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
		vertexData[1].texcoord = { 0.5f,0.0f };
		//右下
		vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
		vertexData[2].texcoord = { 1.0f,1.0f };

		//左下
		vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
		vertexData[3].texcoord = { 0.0f,1.0f };
		//上
		vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
		vertexData[4].texcoord = { 0.5f,0.0f };
		//右下
		vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
		vertexData[5].texcoord = { 1.0f,1.0f };

	}

	//VBV(vertexBufferView)の作成
	void SetVBV()
	{
		//リソースの先頭アドレスから使う
		vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
		//使用する頂点のサイズ
		vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
		//1頂点当たりのサイズ
		vertexBufferView.StrideInBytes = sizeof(VertexData);
	}

	//PSOの作成
	void MakePSO(IDxcBlob* vertexShaderBlob_, IDxcBlob* pixcelShaderBlob_, ID3D12Device* device_)
	{
		//RootSignature
		graghicsPipeLineStatedesc.pRootSignature = rootSignature;
		//InputLayOut
		graghicsPipeLineStatedesc.InputLayout = inputLayoutDesc;
		//VertexShader
		graghicsPipeLineStatedesc.VS =
		{ 
			vertexShaderBlob_->GetBufferPointer(),
			vertexShaderBlob_->GetBufferSize()
		};
		//pixcelShader
		graghicsPipeLineStatedesc.PS =
		{
			pixcelShaderBlob_->GetBufferPointer(),
			pixcelShaderBlob_->GetBufferSize()
		};
		//BlendState
		graghicsPipeLineStatedesc.BlendState = blendDesc;
		//ラスタライザーステート
		graghicsPipeLineStatedesc.RasterizerState = rasterizerDesc;
		//書き込むRTVの情報
		graghicsPipeLineStatedesc.NumRenderTargets = 1;
		graghicsPipeLineStatedesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		//利用するトポロジ（形状）のタイプ三角形
		graghicsPipeLineStatedesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//どのようにして色を打ち込むかの設定
		graghicsPipeLineStatedesc.SampleDesc.Count = 1;
		graghicsPipeLineStatedesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		
		//DepthStencilStateの設定
		//depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		//書き込みする
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		//比較関数はLessEqual。つかり近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		graghicsPipeLineStatedesc.DepthStencilState = depthStencilDesc;
		graghicsPipeLineStatedesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		//実際に生成
		HRESULT hr = device_->CreateGraphicsPipelineState(&graghicsPipeLineStatedesc,
			IID_PPV_ARGS(&graghicsPipelineState));
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete create graghicsPipelineState\n");
	}

	//ラスタライザーの設定
	void SetRasterizerDesc()
	{
		//裏面（時計回り）の表示をしない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		//三角形の中身を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	}

	//BlendStateの設定
	void SetBlendState()
	{
		//すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask =
			D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	//InputLayoutの設定
	void SetInputElementDesc()
	{
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		
		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);
	}

	//BufferResource(example: VertexBuffer,constantbuffer)を作る関数
	ID3D12Resource* CreateBufferResource(ID3D12Device* device_, size_t sizeInByte_)
	{
		ID3D12Resource* ret_resource;
		//頂点リソースのヒープ設定
		//upLoadHEapを使う
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		//頂点リソースの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		//バッファリソース。テクスチャの場合はまた別の設定をする
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		//リソースのサイズ。
		resourceDesc.Width = sizeInByte_;
		//バッファの場合はこれらを1にする決まり
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		//決まり2
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		//実際に頂点リソースを作る
		HRESULT hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&ret_resource));
		assert(SUCCEEDED(hr));

		return ret_resource;
	}



	//RootSignatureの作成
	void MakeRootSignature(ID3D12Device* device_)
	{	
		//バイリニアフィルター
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		//0～1の範囲外をリピート
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		//比較しない
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		//ありったけのmipMapを使う
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		//レジスタ番号0を使う
		staticSamplers[0].ShaderRegister = 0;
		//PixcelShaderで使う
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		descriptionRootSIgnature.pStaticSamplers = staticSamplers;
		descriptionRootSIgnature.NumStaticSamplers = _countof(staticSamplers);
		
		//pixcelShaderで読むConstantBufferのBind情報を追加する
		descriptionRootSIgnature.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
		//RootParameter
		SetRootPara();

		//シリアライズしてバイナリにする
		HRESULT hr = D3D12SerializeRootSignature(
			&descriptionRootSIgnature,
			D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr))
		{
			Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}

		//バイナリをもとにrootSignatureを作成
		hr = device_->CreateRootSignature(0,
			signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete create rootSignature\n");

	}
	

	//コマンドキューの生成
	void MakeCommandQueue(ID3D12Device* device_)
	{
		HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc,
			IID_PPV_ARGS(&commandQueue));
		//コマンドキューの生成がうまくいかなかった場合はエラー
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete create CommandQueue\n");
	}

	//コマンドアローケータの生成
	void MakeCommandAllocator(ID3D12Device* device_)
	{
		HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator));
		//生成がうまくいかなかった場合はエラー
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete create AlloCator\n");
	}

	//コマンドリストを生成する
	void MakeCommandList(ID3D12Device* device_)
	{
		HRESULT hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator,
			nullptr, IID_PPV_ARGS(&commandList));
		//生成失敗
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete create CommandList\n");
	}

	void SetSwapChain(HWND hwnd_,int32_t kClientWidth_, int32_t kClientHeight_ ,IDXGIFactory7* dxgiFactory_)
	{ 
		//画面の縦横。クライアント領域と同じにしておく
		swapChainDesc.Width = kClientWidth_;
		swapChainDesc.Height = kClientHeight_;
		//色の形成
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//マルチサンプルしない
		swapChainDesc.SampleDesc.Count = 1;
		//描画のターゲットとして利用する
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//ダブルバッファ
		swapChainDesc.BufferCount = 2;
		//モニタに移したら中身を破棄
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
		HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue, hwnd_, &swapChainDesc,
			nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
		//生成失敗
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete create SwapChain\n");

	}

	//ディスクリプタヒープの作成
	void PullSwapChainResource(ID3D12Device* device_)
	{
		Log(WindowSetUp::debugLog, "Complete create rtvDescriptorHeap\n");

		//SwapChainからResourceを引っ張ってくる
		HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
		//生成失敗
		assert(SUCCEEDED(hr));
		hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
		//生成失敗
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete pull Resource\n");
	}

	//RTVの設定
	void SetRTV(ID3D12Device* device_)
	{
		//出力結果をSRGBに変換する
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		//2Dテクスチャとして書き込む
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//ディスクリプタの先頭を取得する
		D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		//1つめを作る。１つ目は最初に作る。作る場所をこちらで指定する必要がある
		rtvHandles[0] = rtvStartHandle;
		device_->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
		//2つめのディスクリプタハンドルを得る
		rtvHandles[1].ptr = rtvHandles[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		//2つめを作る
		device_->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);
	}

	void Finalize()
	{
		rtvDescriptorHeap->Release();
		srvDescriptorHeap->Release();
		swapChainResources[0]->Release();
		swapChainResources[1]->Release();
		swapChain->Release();
		commandList->Release();
		commandAllocator->Release();
		commandQueue->Release();
		vertexResource->Release();
		materialResource->Release();
		graghicsPipelineState->Release();
		signatureBlob->Release();
		if(errorBlob) errorBlob->Release();
		rootSignature->Release();
		wvpResource->Release();
		depthStencilTextureResource->Release();
		dsvDescriptorHeap->Release();

	}

};
