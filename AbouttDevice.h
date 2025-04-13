#pragma once

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include "AboutWindow.h"



struct DeviceSetUp
{
	//DXGIファクトリーの作成
	IDXGIFactory7* dxgiFactory = nullptr;
	//使用するアダプタ用の変数。
	IDXGIAdapter4* useAdapter = nullptr;
	//デバイス
	ID3D12Device* device = nullptr;

	void SetGoodDevice()
	{
		//HRESULTはWindows系のエラーコードであり、
		//関数が成功したかどうかをSUCCEEDEDマクロで判定する
		HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
		//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
		//どうにもできない場合が多いのでassertにしておく
		assert(SUCCEEDED(hr));
		//一番いいアダプタを頼む
		for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i)
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
				Log(WindowSetUp::debugLog, ConvertString(std::format(L"Use Adapter:{}\n", adapterDisc.Description)));

				break;
			}
			//ソフトウェアアダプタの場合はなかったことに
			useAdapter = nullptr;
		}

		//適切なアダプタが見つからなかった場合は起動できない
		assert(useAdapter != nullptr);

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
				Log(WindowSetUp::debugLog, std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
				break;
			}
		}

		//デバイスの生成が上手くいかなかったので起動できない
		assert(device != nullptr);
		Log(WindowSetUp::debugLog, "Complete create D3D12Device\n");

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

	}

	void Finalize()
	{
		device->Release();
		useAdapter->Release();
		dxgiFactory->Release();
	}

};