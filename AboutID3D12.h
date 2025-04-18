#pragma once

#include "AboutWindow.h"
#include <dxcapi.h>
#include "VecAndMat.h"
#include "External/DirectXTex/DirectXTex.h"
#include "External/DirectXTex/d3dx12.h"

#include <vector>

struct TextureHandle
{
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

};

struct ID3D12SetUp
{
	//コマンドキュー
	ID3D12CommandQueue* commandQueue = nullptr;
	//コマンドアローケータ
	ID3D12CommandAllocator* commandAllocator = nullptr;
	//コマンドリスト		
	ID3D12GraphicsCommandList* commandList = nullptr;
	//スワップチェーン
	IDXGISwapChain4* swapChain = nullptr;
	//PSO
	ID3D12PipelineState* graghicsPipelineState = nullptr;

	//[ DescriptorHeap ]
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	ID3D12DescriptorHeap* srvDescriptorHeap = nullptr;
	ID3D12DescriptorHeap* dsvDescriptorHeap = nullptr;

	//[ Reaource ]
	ID3D12Resource* swapChainResources[2] = { nullptr };
	ID3D12Resource* intermediateResource = nullptr;
	ID3D12Resource* depthStencilTextureResource = nullptr;

	ID3D12Resource* vertexResource = nullptr;
	ID3D12Resource* vertexSpriteResource = nullptr;

	ID3D12Resource* transformationMatrixSpriteResource = nullptr;
	ID3D12Resource* transformationMatrixResource = nullptr;

	ID3D12Resource* materialSpriteResource = nullptr;
	ID3D12Resource* materialResource = nullptr;

	//[ Handle ]
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	std::vector<TextureHandle >textureHandles;


	//[ View ]
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferSpriteView{};

	//具体的にshaderがどこかでデータを読めばいいのかの情報をまとめたもの
	//shaderとPSOのバインダー
	//RootSignature
	ID3D12RootSignature* rootSignature = nullptr;
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	//複数設定できるので配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//Shaderで扱うViewのDescriptorを設定するために使う
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};

	//[ SomeData ]
	TransformationMatrix* transformationMatrixData = nullptr;
	TransformationMatrix* transformationMatrixSpriteData = nullptr;

	Vector4<float>* materialData = nullptr;
	Material* materialResourceData = nullptr;

	VertexData* vertexData = nullptr;
	VertexData* vertexSpriteData = nullptr;

	inline static uint32_t CPUDescriptorHandleIndex = 0;
	inline static uint32_t GPUDescriptorHandleIndex = 0;



	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descrptorHeap_, uint32_t descriptorSize_);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descrptorHeap_, uint32_t descriptorSize_);
	//ラスタライザーの設定
	D3D12_RASTERIZER_DESC GetRasterizerDesc();
	//DepthStencilViewDesc(DSVDesc)の設定
	D3D12_DEPTH_STENCIL_VIEW_DESC CreateDepthStencilViewDesc(ID3D12Device* device_);
	//ShaderResourceViewDescの生成(とりま適当引数)
	D3D12_SHADER_RESOURCE_VIEW_DESC CreateSRVDesc(DXGI_FORMAT metaDataFormat_, size_t mipLevels_);
	//DescriptorRangeの設定を行う
	void SetShaderViewDescriptorRange();
	//textureデータを転送する関数
	[[nodiscard]]
	ID3D12Resource* UploadtextureData(ID3D12Resource* texture_, DirectX::ScratchImage const& mipImages_,
		ID3D12Device* device_);
	//DepthStencilを生成する
	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device_, int32_t width_, int32_t height_);
	//読み込んだTextureの情報をもとにTextureResourceを作る関数
	ID3D12Resource* CreateTextureResource(ID3D12Device* device_, DirectX::TexMetadata& metaData_);
	//DescriptorHeapの作成関数
	ID3D12DescriptorHeap* MakeDescriptorHeap(ID3D12Device* device_,
		D3D12_DESCRIPTOR_HEAP_TYPE heapType_, UINT numDescriptors_, bool shaderVisible);
	//ConstantBufferを1つ読むための設定（関数内部）
	void SetRootParameter();
	//DXの行列の設定
	void SetDXMatrix(int32_t kClientWidth_, int32_t kClientHeight_, D3D12_VIEWPORT& dst_viewport_, D3D12_RECT dst_scissorRect_);
	//VertexBufferViewの作成
	void SetVertexBufferView(UINT numTriangle_);
	//VertexBufferViewSpriteの作成
	void SetVertexBufferSpriteView(UINT numSprite_);
	//PSOの作成
	void MakePSO(IDxcBlob* vertexShaderBlob_, IDxcBlob* pixcelShaderBlob_, ID3D12Device* device_);
	//BufferResource(example: VertexBuffer,constantbuffer)を作る関数
	ID3D12Resource* CreateBufferResource(ID3D12Device* device_, size_t sizeInByte_);
	//RootSignatureの作成
	void MakeRootSignature(ID3D12Device* device_);
	//コマンドキューの生成
	void MakeCommandQueue(ID3D12Device* device_);
	//コマンドアローケータの生成
	void MakeCommandAllocator(ID3D12Device* device_);
	//コマンドリストを生成する
	void MakeCommandList(ID3D12Device* device_);
	//swapChainの設定
	DXGI_SWAP_CHAIN_DESC1 SetSwapChain(HWND hwnd_, int32_t kClientWidth_, int32_t kClientHeight_, IDXGIFactory7* dxgiFactory_);
	//ディスクリプタヒープの作成
	void PullSwapChainResource(ID3D12Device* device_);
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC SetRenderTargetView(ID3D12Device* device_);


	//マテリアルリソースにデータを書き込む
	void OverrideMaterialData();
	//マテリアルスプタイトリソースにデータを書き込む
	void OverrideMaterialSpriteData();
	//頂点リソースにデータを書き込む
	void OverrideVertexData();
	//スプライト頂点リソースにデータを書き込む
	void OverrideVertexSpriteData();
	//スプライト用のMatrixリソースにデータを書き込む
	void OverrideTransformationMatrixSpriteData();
	//頂点用のMatrixリソースにデータを書き込む
	void OverrideTransformationMatrixData();


	void Finalize();

};
