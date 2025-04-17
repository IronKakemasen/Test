#include "AboutDX.h"

#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Dbghelp.lib")


UINT kNumVertex = 16 * 16 * 6;

void MyDX::Initialize()
{
	//[ ウィンドウ、デバッグ、例外設定？の初期化 ]
	windowSetUp.SetWindowAndSome(1280, 720);

	//[ デバイス ]
	//アダプタを探してデバイスの設定をする
	deviceSetUp.SetGoodDevice();

	//[ フェンス ]
	fenceSetUp.SetFence(deviceSetUp.device, deviceSetUp.dxgiFactory);

	//[ コンパイラーの初期化 ]
	shaderSetUp.SetDXCCompiler();

	//[ ID3D12? ]
	//コマンドキューの生成
	iD3D12SetUp.MakeCommandQueue(deviceSetUp.device);
	//コマンドアローケータの生成
	iD3D12SetUp.MakeCommandAllocator(deviceSetUp.device);
	//コマンドリストを生成する
	iD3D12SetUp.MakeCommandList(deviceSetUp.device);
	//SwapChainの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = iD3D12SetUp.SetSwapChain(windowSetUp.hwnd, windowSetUp.kClientWidth,
		windowSetUp.kClientHeight, deviceSetUp.dxgiFactory);
	

	//RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので,ShaderVisibleはfalse
	iD3D12SetUp.rtvDescriptorHeap = iD3D12SetUp.MakeDescriptorHeap(deviceSetUp.device,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,2, false);
	//SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るもなので,ShaderVisibleはtrue
	iD3D12SetUp.srvDescriptorHeap = iD3D12SetUp.MakeDescriptorHeap(deviceSetUp.device,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	//DSV用のディスクリプタヒープ(Shader内で見るものではない)
	iD3D12SetUp.dsvDescriptorHeap = iD3D12SetUp.MakeDescriptorHeap(deviceSetUp.device,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	//SwapChainResourceからResourceを引っ張ってくる
	iD3D12SetUp.PullSwapChainResource(deviceSetUp.device);
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = iD3D12SetUp.SetRenderTargetView(deviceSetUp.device);

	//[ PSO ]
	//DescriptorRangeの設定
	iD3D12SetUp.SetShaderViewDescriptorRange();
	//RootSignatureの作成
	iD3D12SetUp.MakeRootSignature(deviceSetUp.device);
	//シェーダをコンパイルする
	shaderSetUp.CompileMyShaderes();
	//PSOの作成
	iD3D12SetUp.MakePSO(shaderSetUp.vertexShaderBlob, shaderSetUp.pixcelShaderBlob, deviceSetUp.device);
	
	//VertexResourceの生成
	iD3D12SetUp.vertexResource = iD3D12SetUp.CreateBufferResource(deviceSetUp.device, sizeof(VertexData) * kNumVertex);
	//materilalResourceの生成
	iD3D12SetUp.materialResource = iD3D12SetUp.CreateBufferResource(deviceSetUp.device, sizeof(Vector4<float>));
	//vpvResourceの生成
	iD3D12SetUp.wvpResource = iD3D12SetUp.CreateBufferResource(deviceSetUp.device, sizeof(Matrix4));
	//vertexSpriteResourceの作成
	iD3D12SetUp.vertexSpriteResource = iD3D12SetUp.CreateBufferResource(deviceSetUp.device, sizeof(VertexData) * 6);
	//transformationMatrixSpriteResourceの作成
	iD3D12SetUp.transformationMatrixSpriteResource = iD3D12SetUp.CreateBufferResource(deviceSetUp.device, sizeof(Matrix4) * 6);

	//Textureを読み込んで転送する
	DirectX::ScratchImage mipImages = LoadTexture("Resource/TestImage/uvChecker.png");
	dxTexSetUp.metaData = mipImages.GetMetadata();
	dxTexSetUp.textureResource = iD3D12SetUp.CreateTextureResource(deviceSetUp.device, dxTexSetUp.metaData);
	iD3D12SetUp.intermediateResource = iD3D12SetUp.UploadtextureData(dxTexSetUp.textureResource, mipImages, deviceSetUp.device);

	//metaDataをもとにShaderReaourceViewDescの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = iD3D12SetUp.CreateSRVDesc(dxTexSetUp.metaData.format, dxTexSetUp.metaData.mipLevels);

	//depthStencilresourceの作成
	iD3D12SetUp.depthStencilTextureResource = iD3D12SetUp.CreateDepthStencilTextureResource(
		deviceSetUp.device, windowSetUp.kClientWidth, windowSetUp.kClientHeight);

	//VBV(vertexBufferView)の設定
	iD3D12SetUp.SetVertexBufferView(kNumVertex);
	iD3D12SetUp.SetVertexBufferSpriteView(1);

	//DepthStencilViewDescの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = iD3D12SetUp.CreateDepthStencilViewDesc(deviceSetUp.device);
	//DSVHeapの先頭にDSVを作る
	deviceSetUp.device->CreateDepthStencilView(iD3D12SetUp.depthStencilTextureResource, &depthStencilViewDesc,
		iD3D12SetUp.dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


	//頂点リソースにデータを書き込む
	iD3D12SetUp.OverrideVertexData();
	//マテリアルリソースにデータを書き込む
	iD3D12SetUp.OverrideMaterialData();
	//vpvResourceにデータを書き込む
	iD3D12SetUp.OverrideWVPData();
	//スプライト頂点リソースにデータを書き込む
	iD3D12SetUp.OverrideVertexSpriteData();
	//スプライト用のMatrixリソースにデータを書き込む
	iD3D12SetUp.OverrideMatrixSpriteData();


	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	ImGui_ImplWin32_Init(windowSetUp.hwnd);
	ImGui_ImplDX12_Init(
		deviceSetUp.device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		iD3D12SetUp.srvDescriptorHeap,
		iD3D12SetUp.srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		iD3D12SetUp.srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	//SRVを作成するDescriptorHeapの場所を決める
	iD3D12SetUp.textureSrvHandleCPU =
		iD3D12SetUp.srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	iD3D12SetUp.textureSrvHandleGPU =
		iD3D12SetUp.srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	
	//先頭はImGuiが使っているのでその次を使う
	iD3D12SetUp.textureSrvHandleCPU.ptr +=
		deviceSetUp.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	iD3D12SetUp.textureSrvHandleGPU.ptr +=
		deviceSetUp.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	//SRVの作成
	deviceSetUp.device->CreateShaderResourceView(dxTexSetUp.textureResource, &srvDesc,
		iD3D12SetUp.textureSrvHandleCPU);
	

}
void MyDX::Update()
{
	//Imguiにここからフレームが始まる旨を告げる
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
	ImGui::Render();

	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = iD3D12SetUp.swapChain->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	//今回のバリアはTransition
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファーに対して行う
	barrier.Transition.pResource = iD3D12SetUp.swapChainResources[backBufferIndex];
	//遷移前（現在）のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	iD3D12SetUp.commandList->ResourceBarrier(1, &barrier);
	//描画先のRTV、DSVを設定する
	iD3D12SetUp.dsvHandle = iD3D12SetUp.dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	iD3D12SetUp.commandList->OMSetRenderTargets(1, &iD3D12SetUp.rtvHandles[backBufferIndex], false, &iD3D12SetUp.dsvHandle);

	//指定した深度で画面クリアする
	iD3D12SetUp.commandList->ClearDepthStencilView(iD3D12SetUp.dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	

	//<<<<<<<<<<<<<<<<<<<<<<<画面に書く>>>>>>>>>>>>>>>>>>>>>>>>>
	iD3D12SetUp.commandList->ClearRenderTargetView(iD3D12SetUp.rtvHandles[backBufferIndex], clearColor, 0, nullptr);
	////描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { iD3D12SetUp.srvDescriptorHeap };
	iD3D12SetUp.commandList->SetDescriptorHeaps(1, descriptorHeaps);

	//DXの行列の設定
	D3D12_VIEWPORT viewPort{};
	D3D12_RECT scissorRect{};
	iD3D12SetUp.SetDXMatrix(windowSetUp.kClientWidth, windowSetUp.kClientHeight, viewPort, scissorRect);
	iD3D12SetUp.commandList->RSSetViewports(1, &viewPort);
	iD3D12SetUp.commandList->RSSetScissorRects(1, &scissorRect);

	//RootSignatureを設定、PSOに設定しているけど別途必要
	iD3D12SetUp.commandList->SetGraphicsRootSignature(iD3D12SetUp.rootSignature);
	iD3D12SetUp.commandList->SetPipelineState(iD3D12SetUp.graghicsPipelineState);
	//VBV
	iD3D12SetUp.commandList->IASetVertexBuffers(0, 1, &iD3D12SetUp.vertexBufferView);
	//形状の設定。PSOに設定しているものと同じものを選択
	iD3D12SetUp.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルのCバッファの場所を指定
	iD3D12SetUp.commandList->SetGraphicsRootConstantBufferView(0, iD3D12SetUp.materialResource->GetGPUVirtualAddress());
	//wvp用Cバッファの場所を指定
	iD3D12SetUp.commandList->SetGraphicsRootConstantBufferView(1, iD3D12SetUp.wvpResource->GetGPUVirtualAddress());
	//SRVのDescriptortableの先頭を設定。2はrootparameter[2]である
	iD3D12SetUp.commandList->SetGraphicsRootDescriptorTable(2, iD3D12SetUp.textureSrvHandleGPU);
	//描画(DrawCall)。3頂点で一つのインスタンス
	iD3D12SetUp.commandList->DrawInstanced(kNumVertex, 1, 0, 0);
	
	////spriteの描画
	//iD3D12SetUp.commandList->IASetVertexBuffers(0, 1, &iD3D12SetUp.vertexBufferSpriteView);
	////TransformationMatrixCBufferの場所を設定
	//iD3D12SetUp.commandList->SetGraphicsRootConstantBufferView(1, iD3D12SetUp.transformationMatrixSpriteResource->GetGPUVirtualAddress());
	////描画
	//iD3D12SetUp.commandList->DrawInstanced(6, 1, 0, 0);


	//画面に書く処理が終わり、画面に映すので、状態を遷移
	//RenderTarget->Prsent
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	//実際のcommandListのImguiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), iD3D12SetUp.commandList);

	//TransitionBarrierを張る
	iD3D12SetUp.commandList->ResourceBarrier(1, &barrier);

	//コマンドリストの内容を確定させる
	HRESULT hr = iD3D12SetUp.commandList->Close();
	assert(SUCCEEDED(hr));
	Log(windowSetUp.debugLog, "Complete stack command\n");

	//GPUにコマンドリストの実行を行わさせる
	ID3D12CommandList* commandLists[] = { iD3D12SetUp.commandList };
	iD3D12SetUp.commandQueue->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うように通知する
	iD3D12SetUp.swapChain->Present(1, 0);

	//Fenceの値を更新
	fenceSetUp.fenceValue++;
	//GPUがここまでたどり着いたとき、
	//fenceの値を指定した値に代入するようにSignalを送る
	iD3D12SetUp.commandQueue->Signal(fenceSetUp.fence, fenceSetUp.fenceValue);
	//fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompletedValueの初期値はfence作成時に渡した初期値
	if (fenceSetUp.fence->GetCompletedValue() < fenceSetUp.fenceValue)
	{
		//指定したSignalにたどり着いていないので、たどり着くまで待つ
		fenceSetUp.fence->SetEventOnCompletion(fenceSetUp.fenceValue, fenceSetUp.fenceEvent);
		//イベント待つ
		WaitForSingleObject(fenceSetUp.fenceEvent, INFINITE);
		Log(windowSetUp.debugLog, "mattawayo\n");
	}

	//次のフレーム用のコマンドリストを準備
	hr = iD3D12SetUp.commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = iD3D12SetUp.commandList->Reset(iD3D12SetUp.commandAllocator, nullptr);;
	assert(SUCCEEDED(hr));
	//Log(windowSetUp.debugLog, "Complete kickAndReset command\n");

}

void MyDX::Finalize()
{
	ImGuiFinalize();

	//解放
	fenceSetUp.Finalize();
	iD3D12SetUp.Finalize();
	deviceSetUp.Finalize();
	shaderSetUp.Finalize();

	//リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}

	//#ifdef _DEBUG
	//	debugController
	//#endif
	CloseWindow(windowSetUp.hwnd);
	Log(windowSetUp.debugLog, "Complete close window\n");

}
