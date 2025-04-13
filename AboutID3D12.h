#pragma once

#include "AboutWindow.h"
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include "VecAndMat.h"

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
	
	//ディスクリプタヒープ
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	
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
	D3D12_ROOT_PARAMETER rootParameters[1] = {};


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
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

	//VBV
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//ViewPortMat
	D3D12_VIEWPORT viewPort{};
	//シザー矩形
	D3D12_RECT scissorRect{};
	

	//ConstantBufferを1つ読むための設定（関数内部）
	void SetRootPara()
	{
		//CBVを使う(b0のb)
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		//PixcelShaderで使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//レジスタ番号0とバインド
		rootParameters[0].Descriptor.ShaderRegister = 0;
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

	//マテリアルリソースにデータを書き込む
	void OverrideMaterialData()
	{
		Vec4<float>* materialData = nullptr;

		//書き込むためのアドレスを取得
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
		//色
		*materialData = { 1.0f,0.0f,0.0f,1.0f };

	}

	//頂点リソースにデータを書き込む
	void OverrideVertexData()
	{
		Vec4<float>* vertexData = nullptr;

		//書き込むためのアドレスを取得
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
		//左下
		vertexData[0] = {-0.5f,-0.5f,0.0f,1.0f};
		//上
		vertexData[1] = { 0.0f,0.5f,0.0f,1.0f };
		//右下
		vertexData[2] = { 0.5f,-0.5f,0.0f,1.0f };

	}


	//VBV(vertexBufferView)の作成
	void MakeVBV()
	{
		//リソースの先頭アドレスから使う
		vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
		//使用する頂点のサイズは3つ
		vertexBufferView.SizeInBytes = sizeof(Vec4<float>) * 3;
		//1頂点当たりのサイズ
		vertexBufferView.StrideInBytes = sizeof(Vec4<float>);
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
		//リソースのサイズ。今回はVecter4を4頂点分
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
		//descriptionRootSIgnature.Flags =
		//	D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
		//pixcelShaderで読むConstantBufferのBind情報を追加する
		descriptionRootSIgnature.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
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

		//バイナリをもとに作成
		rootSignature = nullptr;
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
	void MakeDescriptorHeap(ID3D12Device* device_)
	{
		//レンダーターゲットビュー用
		rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		//ダブルバッファ用に2つ
		rtvDescriptorHeapDesc.NumDescriptors = 2;
		HRESULT hr = device_->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
		//生成失敗
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete create DescriptorHeap\n");

		//SwapChainからResourceを引っ張ってくる
		hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
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
	}

};
