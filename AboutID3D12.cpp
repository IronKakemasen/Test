#include "AboutID3D12.h"



D3D12_CPU_DESCRIPTOR_HANDLE ID3D12SetUp::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descrptorHeap_, 
	uint32_t descriptorSize_)
{
	D3D12_CPU_DESCRIPTOR_HANDLE ret_handleCPU = descrptorHeap_->GetCPUDescriptorHandleForHeapStart();
	ret_handleCPU.ptr += descriptorSize_ * CPUDescriptorHandleIndex;

	CPUDescriptorHandleIndex++;

	return ret_handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE ID3D12SetUp::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descrptorHeap_,
	uint32_t descriptorSize_)
{
	D3D12_GPU_DESCRIPTOR_HANDLE ret_handleGPU = descrptorHeap_->GetGPUDescriptorHandleForHeapStart();
	ret_handleGPU.ptr += descriptorSize_ * GPUDescriptorHandleIndex;

	GPUDescriptorHandleIndex++;

	return ret_handleGPU;
}


//スプライト用のMatrixリソースにデータを書き込む
void ID3D12SetUp::OverrideMatrixSpriteData()
{
	//書き込むためのアドレスを取得
	transformationMatrixSpriteResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixSpriteData));
	//単位行列を書き込んでおく
	Matrix4 tmp;
	*transformationMatrixSpriteData = tmp;
}


//ShaderResourceViewDescの生成(とりま適当引数)
D3D12_SHADER_RESOURCE_VIEW_DESC ID3D12SetUp::CreateSRVDesc(DXGI_FORMAT metaDataFormat_, size_t mipLevels_)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaDataFormat_;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(mipLevels_);

	return srvDesc;
}


//DescriptorRangeの設定を行う
void ID3D12SetUp::SetShaderViewDescriptorRange()
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
[[nodiscard]]
ID3D12Resource* ID3D12SetUp::UploadtextureData(ID3D12Resource* texture_, DirectX::ScratchImage const& mipImages_,
	ID3D12Device* device_)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device_, mipImages_.GetImages(),
		mipImages_.GetImageCount(), mipImages_.GetMetadata(), subresource);

	uint64_t intermediateSize = GetRequiredIntermediateSize(texture_, 0, UINT(subresource.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device_, intermediateSize);
	UpdateSubresources(commandList, texture_, intermediateResource, 0, 0,
		UINT(subresource.size()), subresource.data());
	//Textureへの転送後は利用できるよう、D3D2_RESOUTRCE_STATE_COPY_DESTから
	//D3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture_;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;

}

//DepthStencilViewDesc(DSVDesc)の設定
D3D12_DEPTH_STENCIL_VIEW_DESC ID3D12SetUp::CreateDepthStencilViewDesc(ID3D12Device* device_)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC ret_dsvDesc{};

	//format。基本的にはリソースに合わせる
	ret_dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//2DTexture
	ret_dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	return ret_dsvDesc;
}

//DepthStencilを生成する
ID3D12Resource* ID3D12SetUp::CreateDepthStencilTextureResource(ID3D12Device* device_, int32_t width_, int32_t height_)
{
	//生成するResourceの設定
	//テクスチャのサイズ
	D3D12_RESOURCE_DESC resourceDesc{};

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
	D3D12_CLEAR_VALUE depthClearValue{};
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


//DescriptorHeapの作成関数
ID3D12DescriptorHeap* ID3D12SetUp::MakeDescriptorHeap(ID3D12Device* device_,
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
void ID3D12SetUp::SetRootParameter()
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

}

//DXの行列の設定
void ID3D12SetUp::SetDXMatrix(int32_t kClientWidth_, int32_t kClientHeight_, D3D12_VIEWPORT& dst_viewport_, D3D12_RECT dst_scissorRect_)
{
	//クライアント領域と一緒のサイズにして画面全体に表示
	dst_viewport_.Width = static_cast<FLOAT>(kClientWidth_);
	dst_viewport_.Height = static_cast<FLOAT>(kClientHeight_);
	dst_viewport_.TopLeftX = 0;
	dst_viewport_.TopLeftY = 0;
	dst_viewport_.MinDepth = 0.0f;
	dst_viewport_.MaxDepth = 1.0f;
	//しざー矩形
	dst_scissorRect_.right = static_cast<LONG>(kClientWidth_);
	dst_scissorRect_.bottom = static_cast<LONG>(kClientHeight_);
	dst_scissorRect_.left = 0;
	dst_scissorRect_.top = 0;
}

//wvpリソースにデータを書き込む
void ID3D12SetUp::OverrideWVPData()
{
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	Matrix4 tmp;
	*wvpData = tmp;
}

//マテリアルスプライトリソースにデータを書き込む
void ID3D12SetUp::OverrideMaterialSpriteData()
{
	//書き込むためのアドレスを取得
	materialSpriteResource->Map(0, nullptr, reinterpret_cast<void**>(&materialResourceData));
	//色
	materialResourceData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialResourceData->enableLighting = false;
}



//マテリアルリソースにデータを書き込む
void ID3D12SetUp::OverrideMaterialData()
{
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//色
	*materialData = { 1.0f,1.0f,1.0f,1.0f };
}

//頂点リソースにデータを書き込む
void ID3D12SetUp::OverrideVertexData()
{
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	float const kSubdivision{16};
	float const delta_t = Torima::kPi / kSubdivision;
	float const delta_p = Torima::kPi * 2.0f / kSubdivision;

	int vertexSum{0};

	//経度の方向に分割(-0.5Pi <= cur_t <= 0.5Pi)
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		//currentLat
		float cur_t = -Torima::kPi * 0.5f + delta_t * latIndex;

		//緯度の方向に分割(0.0 <= cur_p <= 2Pi)
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			//currentLon
			float cur_p = delta_p * lonIndex;

			int start = int((latIndex * kSubdivision + lonIndex) * 6);

			float u = float(lonIndex / kSubdivision);
			float v = 1.0f - float(latIndex / kSubdivision);

			//a
			vertexData[start].position =
			{
				cosf(cur_t) * cosf(cur_p),
				sinf(cur_t),
				cosf(cur_t) * sinf(cur_p),
				1.0f
			};
			vertexData[start].texcoord = 
			{ 
				float(lonIndex / kSubdivision),
				1.0f - float(latIndex / kSubdivision) 
			};


			//b
			vertexData[start + 1].position =
			{
				cosf(cur_t + delta_t) * cosf(cur_p),
				sinf(cur_t + delta_t),
				cosf(cur_t + delta_t) * sinf(cur_p),
				1.0f,
			};
			vertexData[start + 1].texcoord =
			{
				float(lonIndex / kSubdivision),
				1.0f - float((latIndex + 1) / kSubdivision)
			};

			//c
			vertexData[start + 2].position =
			{
				cosf(cur_t) * cosf(cur_p + delta_p) ,
				sinf(cur_t) ,
				cosf(cur_t) * sinf(cur_p + delta_p) ,
				1.0f
			};
			vertexData[start + 2].texcoord =
			{
				float((lonIndex + 1) / kSubdivision),
				1.0f - float(latIndex  / kSubdivision)
			};



			//c
			vertexData[start + 3] = vertexData[start + 2];

			//b
			vertexData[start + 4] = vertexData[start + 1];

			//d
			vertexData[start + 5].position =
			{
				cosf(cur_t + delta_t) * cosf(cur_p + delta_p) ,
				sinf(cur_t + delta_t) ,
				cosf(cur_t + delta_t) * sinf(cur_p + delta_p) ,
				1.0f,
			};
			vertexData[start + 5].texcoord =
			{
				float((lonIndex + 1) / kSubdivision),
					1.0f - float((latIndex + 1) / kSubdivision)
			};

			for (int i = 0; i < 6; ++i)
			{
				vertexData[start + i].normal.x = vertexData[start + i].position.x;
				vertexData[start + i].normal.y = vertexData[start + i].position.y;
				vertexData[start + i].normal.z = vertexData[start + i].position.z;

			}

		}

	}

	//vertexの数
	Log(WindowSetUp::debugLog, std::format("vertexSum:{}\n", std::to_string(vertexSum)));


}

void ID3D12SetUp::OverrideVertexSpriteData()
{
	//書き込むためのアドレスを取得
	vertexSpriteResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexSpriteData));

	//左下
	vertexSpriteData[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexSpriteData[0].texcoord = { 0.0f,1.0f };
	//左上
	vertexSpriteData[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexSpriteData[1].texcoord = { 0.0f,0.0f };
	//右下
	vertexSpriteData[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexSpriteData[2].texcoord = { 1.0f,1.0f };

	//左上
	vertexSpriteData[3].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexSpriteData[3].texcoord = { 0.0f,0.0f };
	//右上
	vertexSpriteData[4].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexSpriteData[4].texcoord = { 1.0f,0.0f };
	//右下
	vertexSpriteData[5].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexSpriteData[5].texcoord = { 1.0f,1.0f };

}


//VertexBufferView(vertexBufferView)の作成
void ID3D12SetUp::SetVertexBufferView(UINT numVertexes_)
{
	//リソースの先頭アドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用する頂点のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * numVertexes_;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void ID3D12SetUp::SetVertexBufferSpriteView(UINT numSprite_)
{
	//リソースの先頭アドレスから使う
	vertexBufferSpriteView.BufferLocation = vertexSpriteResource->GetGPUVirtualAddress();
	//使用する頂点のサイズ
	vertexBufferSpriteView.SizeInBytes = sizeof(VertexData) * 6 * numSprite_;
	//1頂点当たりのサイズ
	vertexBufferSpriteView.StrideInBytes = sizeof(VertexData);
}


//PSOの作成
void ID3D12SetUp::MakePSO(IDxcBlob* vertexShaderBlob_, IDxcBlob* pixcelShaderBlob_, ID3D12Device* device_)
{
	//graphiscPiPeLineDesc
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graghicsPipeLineStatedesc{};

	//RootSignature
	graghicsPipeLineStatedesc.pRootSignature = rootSignature;
	//InputLayOut
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
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

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	graghicsPipeLineStatedesc.BlendState = blendDesc;
	//ラスタライザーステート
	D3D12_RASTERIZER_DESC rasterizerDesc = GetRasterizerDesc();
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
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
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
D3D12_RASTERIZER_DESC ID3D12SetUp::GetRasterizerDesc()
{
	D3D12_RASTERIZER_DESC ret_rasterizerDesc{};

	//裏面（時計回り）の表示をしない
	ret_rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中身を塗りつぶす
	ret_rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	return ret_rasterizerDesc;
}

//BufferResource(example: VertexBuffer,constantbuffer)を作る関数
ID3D12Resource* ID3D12SetUp::CreateBufferResource(ID3D12Device* device_, size_t sizeInByte_)
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
void ID3D12SetUp::MakeRootSignature(ID3D12Device* device_)
{
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

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

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.pStaticSamplers = staticSamplers;
	rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
	//ルートパラメータ配列へのポインタ
	rootSignatureDesc.pParameters = rootParameters;
	//配列の長さ
	rootSignatureDesc.NumParameters = _countof(rootParameters);
	//pixcelShaderで読むConstantBufferのBind情報を追加する
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter
	SetRootParameter();

	//シリアライズしてバイナリにする
	HRESULT hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,
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
void ID3D12SetUp::MakeCommandQueue(ID3D12Device* device_)
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成がうまくいかなかった場合はエラー
	assert(SUCCEEDED(hr));
	Log(WindowSetUp::debugLog, "Complete create CommandQueue\n");
}

//コマンドアローケータの生成
void ID3D12SetUp::MakeCommandAllocator(ID3D12Device* device_)
{
	HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	//生成がうまくいかなかった場合はエラー
	assert(SUCCEEDED(hr));
	Log(WindowSetUp::debugLog, "Complete create AlloCator\n");
}

//コマンドリストを生成する
void ID3D12SetUp::MakeCommandList(ID3D12Device* device_)
{
	HRESULT hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator,
		nullptr, IID_PPV_ARGS(&commandList));
	//生成失敗
	assert(SUCCEEDED(hr));
	Log(WindowSetUp::debugLog, "Complete create CommandList\n");
}

//swapChainの設定
DXGI_SWAP_CHAIN_DESC1 ID3D12SetUp::SetSwapChain(HWND hwnd_, int32_t kClientWidth_, int32_t kClientHeight_, IDXGIFactory7* dxgiFactory_)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

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

	return swapChainDesc;
}

//ディスクリプタヒープの作成
void ID3D12SetUp::PullSwapChainResource(ID3D12Device* device_)
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
D3D12_RENDER_TARGET_VIEW_DESC ID3D12SetUp::SetRenderTargetView(ID3D12Device* device_)
{
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
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

	return rtvDesc;
}

void ID3D12SetUp::Finalize()
{
	rtvDescriptorHeap->Release();
	srvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	graghicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) errorBlob->Release();
	rootSignature->Release();
	dsvDescriptorHeap->Release();

	depthStencilTextureResource->Release();
	wvpResource->Release();
	intermediateResource->Release();
	vertexSpriteResource->Release();
	transformationMatrixSpriteResource->Release();
	materialSpriteResource->Release();
	vertexResource->Release();
	materialResource->Release();

}
