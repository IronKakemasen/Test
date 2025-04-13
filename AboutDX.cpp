#include "AboutDX.h"

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
	iD3D12SetUp.SetSwapChain(windowSetUp.hwnd, windowSetUp.kClientWidth,
		windowSetUp.kClientHeight, deviceSetUp.dxgiFactory);
	//ディスクリプタヒープの作成
	iD3D12SetUp.MakeDescriptorHeap(deviceSetUp.device);
	//RTVの設定
	iD3D12SetUp.SetRTV(deviceSetUp.device);

	//[ PSO ]
	//RootSignatureの作成
	iD3D12SetUp.MakeRootSignature(deviceSetUp.device);
	//InputLayoutの設定
	iD3D12SetUp.SetInputElementDesc();
	//BlendStateの設定
	iD3D12SetUp.SetBlendState();
	//ラスタライザーの設定
	iD3D12SetUp.SetRasterizerDesc();
	//シェーダをコンパイルする
	shaderSetUp.CompileMyShaderes();
	//PSOの作成
	iD3D12SetUp.MakePSO(shaderSetUp.vertexShaderBlob, shaderSetUp.pixcelShaderBlob, deviceSetUp.device);
	
	//DXの行列の設定
	iD3D12SetUp.SetDXMatrix(windowSetUp.kClientWidth, windowSetUp.kClientHeight);
	//VertexResourceの生成
	iD3D12SetUp.MakeVertexResource(deviceSetUp.device);
	//VBV(vertexBufferView)の作成
	iD3D12SetUp.MakeVBV();
	//頂点リソースにデータを書き込む
	iD3D12SetUp.OverrideVertexData();



}
void MyDX::Update()
{
	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = iD3D12SetUp.swapChain->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
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

	//描画先のRTVを設定する
	iD3D12SetUp.commandList->OMSetRenderTargets(1, &iD3D12SetUp.rtvHandles[backBufferIndex],
		false, nullptr);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	
	
	



	//<<<<<<<<<<<<<<<<<<<<<<<画面に書く>>>>>>>>>>>>>>>>>>>>>>>>>
	iD3D12SetUp.commandList->ClearRenderTargetView(iD3D12SetUp.rtvHandles[backBufferIndex], clearColor, 0, nullptr);
	iD3D12SetUp.commandList->RSSetViewports(1, &iD3D12SetUp.viewPort);
	iD3D12SetUp.commandList->RSSetScissorRects(1, &iD3D12SetUp.scissorRect);
	//RootSignatureを設定、PSOに設定しているけど別途必要
	iD3D12SetUp.commandList->SetGraphicsRootSignature(iD3D12SetUp.rootSignature);
	//VBV
	iD3D12SetUp.commandList->IASetVertexBuffers(0, 1, &iD3D12SetUp.vertexBufferView);
	iD3D12SetUp.commandList->SetPipelineState(iD3D12SetUp.graghicsPipelineState);
	//形状の設定。PSOに設定しているものと同じものを選択
	iD3D12SetUp.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//描画(DrawCall)。3頂点で一つのインスタンス
	iD3D12SetUp.commandList->DrawInstanced(3, 1, 0, 0);











	//画面に書く処理が終わり、画面に映すので、状態を遷移
	//RenderTarget->Prsent
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
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
