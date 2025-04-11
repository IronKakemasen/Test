#include <Windows.h>
#include <cstdint>
#include "AboutDebugLog.h"
#include "AboutException.h"
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <cassert>
#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")



//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
	WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);

		return 0;
	}

	//標準メッセージの処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);

}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	//*********************************デバッグヘルパー************************************************************
	//誰も捕捉しなかった場合に(Unhandled)、補足する関数を登録
	//main関数始まってすぐに登録するといい
	SetUnhandledExceptionFilter(ExportDump);



	//ーーーーーーーーーーーーーーーーーーーーLogーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
	std::filesystem::create_directory("DebugLog");
	std::ofstream tmp = DebugLogInitialize();

	//------------------------------------意味不ゾーン-------------------------------------------------------------
	WNDCLASS wc{};
	//ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wc.lpszClassName = L"TestKun";
	//インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//ウィンドウクラスを登録する
	RegisterClass(&wc);

	//クライアントの領域サイズ
	int32_t const kClientWidth = 1280;
	int32_t const kClientHeight = 720;

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };
	//クライアント領域をもとに実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,		//利用するクラス名
		L"CG2",					//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル
		CW_USEDEFAULT,			//表示x座標
		CW_USEDEFAULT,			//表示y座標
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		wc.hInstance,			//インスタンスハンドル
		nullptr);				//オプション

	//ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);
	
	//DXGIファクトリーの作成
	IDXGIFactory7* dxgiFactory = nullptr;
	//HRESULTはWindows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判定する
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	//どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));
	//使用するアダプタ用の変数。
	IDXGIAdapter4* useAdapter = nullptr;
	//一番いいアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter))!=DXGI_ERROR_NOT_FOUND;++i)
	{
		//アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDisc{};
		hr = useAdapter->GetDesc3(&adapterDisc);
		//取得できなければエラー
		assert(SUCCEEDED(hr));
		//ソフトウェアアダプタでなければ採用
		if (!(adapterDisc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//採用したアダプタの情報をログに出力
			Log(tmp, ConvertString(std::format(L"Use Adapter:{}\n", adapterDisc.Description)));
			break;
		}
		//ソフトウェアアダプタの場合はなかったことに
		useAdapter = nullptr;
	}

	//適切なアダプタが見つからなかった場合は起動できない
	assert(useAdapter != nullptr);

	ID3D12Device* device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};
	std::string featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		//採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr))
		{
			//生成できたのでログを出力してループを抜ける
			Log(tmp, std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
			break;
		}
	}

	//デバイスの生成が上手くいかなかったので起動できない
	assert(device != nullptr);
	Log(tmp,"Complete create D3D12Device\n");
	//エラー解消のために
#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		//ヤバいエラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] =
		{
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤー
			//相互作用バグによるエラーメッセージ
			//http://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);


		//解放
		infoQueue->Release();
	}

#endif

	//メインループ開始前?にフェンスを作る
	//初期値0
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
	Log(tmp, "Complete built fence\n");

	//FenceのSignalを待つためのイベント（WINDOWへのメッセージ）を作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
	Log(tmp, "Complete creat event\n");



	//コマンドキューの生成
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成がうまくいかなかった場合はエラー
	assert(SUCCEEDED(hr));
	Log(tmp, "Complete create CommandQueue\n");


	//コマンドアローケータの生成
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	//生成がうまくいかなかった場合はエラー
	assert(SUCCEEDED(hr));
	Log(tmp, "Complete create AlloCator\n");


	//コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator,
		nullptr, IID_PPV_ARGS(&commandList));
	//生成失敗
	assert(SUCCEEDED(hr));
	Log(tmp, "Complete create CommandList\n");


	//SwapChainの作成
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//画面の縦横。クライアント領域と同じにしておく
	swapChainDesc.Width = kClientWidth;		
	swapChainDesc.Height = kClientHeight;
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
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	//生成失敗
	assert(SUCCEEDED(hr));
	Log(tmp, "Complete create SwapChain\n");


	//ディスクリプタヒープの作成
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	//レンダーターゲットビュー用
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	//ダブルバッファ用に2つ
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
	//生成失敗
	assert(SUCCEEDED(hr));
	Log(tmp, "Complete create DescriptorHeap\n");

	//SwapChainからResourceを引っ張ってくる
	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	//生成失敗
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	//生成失敗
	assert(SUCCEEDED(hr));
	Log(tmp, "Complete pull Resource\n");


	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//出力結果をSRGBに変換する
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//2Dテクスチャとして書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//RTVを2つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//1つめを作る。１つ目は最初に作る。作る場所をこちらで指定する必要がある
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	//2つめのディスクリプタハンドルを得る
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つめを作る
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);


	//--------------------+-+--
	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	
	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファーに対して行う
	barrier.Transition.pResource = swapChainResources[backBufferIndex];
	//遷移前（現在）のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);
	
	//描画先のRTVを設定する
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex],
		false, nullptr);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	//画面に書く
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
	
	//画面に書く処理が終わり、画面に映すので、状態を遷移
	//RenderTarget->Prsent
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	//コマンドリストの内容を確定させる
	hr = commandList->Close();
	assert(SUCCEEDED(hr));
	Log(tmp, "Complete stack command\n");
	
	//GPUにコマンドリストの実行を行わさせる
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うように通知する
	swapChain->Present(1, 0);

	//Fenceの値を更新
	fenceValue++;
	//GPUがここまでたどり着いたとき、
	//fenceの値を指定した値に代入するようにSignalを送る
	commandQueue->Signal(fence, fenceValue);
	//fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompletedValueの初期値はfence作成時に渡した初期値
	if (fence->GetCompletedValue() < fenceValue)
	{
		//指定したSignalにたどり着いていないので、たどり着くまで待つ
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		//イベント待つ
		WaitForSingleObject(fenceEvent, INFINITE);
		Log(tmp, "mattawayo\n");
	}

	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);;
	assert(SUCCEEDED(hr));
	Log(tmp, "Complete kickAndReset command\n");

	//--------------------+-+--

	//------------------------------------意味不ゾーン-------------------------------------------------------------















	MSG msg{};
	//ウィンドウのxが押されるまでループ
	while (msg.message != WM_QUIT)
	{
		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);


		}

		else
		{
			//ゲームの処理

		}

	}

	//解放
	CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();

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
	CloseWindow(hwnd);


	return 0;
}