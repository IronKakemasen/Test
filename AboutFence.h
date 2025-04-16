#pragma once

#include <dxgidebug.h>
#include "AboutWindow.h"

struct FenceSetUp
{
	//メインループ開始前?にフェンスを作る
	//初期値0
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent = nullptr;

	void MakeFenceEvent()
	{
		//FenceのSignalを待つためのイベント（WINDOWへのメッセージ）を作成する
		fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr);
		Log(WindowSetUp::debugLog, "Complete creat event\n");
	}

	void SetFence(ID3D12Device* device_, IDXGIFactory7* dxgiFactory)
	{
		HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
		hr = device_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&fence));
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete built fence\n");

		MakeFenceEvent();
	}

	void Finalize()
	{
		CloseHandle(fenceEvent);
		fence->Release();
	}


};