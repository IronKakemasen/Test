#pragma once

#include "AboutWindow.h"
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")


struct ShaderCompileSetUp
{
	//dxCompiler
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;
	
	//IDxcBlob 
	IDxcBlob* vertexShaderBlob = nullptr;
	IDxcBlob* pixcelShaderBlob = nullptr;


	void SetDXCCompiler()
	{
		HRESULT hr = DxcCreateInstance(CLSID_DxcUtils,IID_PPV_ARGS(&dxcUtils));
		assert(SUCCEEDED(hr));
		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
		assert(SUCCEEDED(hr));
		hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
		assert(SUCCEEDED(hr));
	}

	//対象のシェーダーをコンパイルする関数
	IDxcBlob* CompileShader(
		//コンパイルするシェーダのパス
		std::wstring const& filePath_,
		//コンパイルに使用するプロファイル
		const wchar_t* profile_,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils_,
		IDxcCompiler3* dxcCompiler_,
		IDxcIncludeHandler* includeHandler_)
	{
		//[ 1.hlslファイルを読み込む ]
		Log(ConvertString(std::format(L"Begin CompileShader, path:{},profile:{}\n", filePath_, profile_)));
		IDxcBlobEncoding* shaderSource = nullptr;
		HRESULT hr = dxcUtils_->LoadFile(filePath_.c_str(), nullptr, &shaderSource);
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete load shader\n");

		//読み込んだファイルの内容を設定する
		DxcBuffer shaderSourceBuffer;
		shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
		shaderSourceBuffer.Size = shaderSource->GetBufferSize();
		//UTF-8のもじであることを通知
		shaderSourceBuffer.Encoding = DXC_CP_UTF8;

		//[ 2.コンパイルする ]
		LPCWSTR arguments[] =
		{
			//コンパイル対象のhlslファイル名
			filePath_.c_str(),
			//エントリーポイントの指定。基本的にmain以外にしない
			L"-E",L"main",
			//shaderProfileの設定
			L"-T",profile_,
			//デバッグ用の情報を埋め込む
			L"-Zi",L"-Qembed_debug",
			//最適化を外す
			L"-Od",
			//メモリレイアウトは行優先
			L"-Zpr"
		};

		IDxcResult* shaderResult = nullptr;
		hr = dxcCompiler_->Compile(
			//読み込んだファイル
			&shaderSourceBuffer,
			//コンパイルオプション
			arguments,
			//コンパイルオプションの数
			_countof(arguments),
			//includeが含まれたもろもろ
			includeHandler_,
			IID_PPV_ARGS(&shaderResult)
		);
		//dxcが起動できないなどの致命的な状況
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, "Complete compile\n");

		//[ 3.エラー、警告確認 ]
		//警告エラーが出てたらログに出して止める
		IDxcBlobUtf8* shaderError = nullptr;
		shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
		if (shaderError != nullptr && shaderError->GetStringLength() != 0)
		{
			Log(shaderError->GetStringPointer());
			assert(false);
		}

		//[ 4.コンパイル結果を受け取る ]
		//コンパイル結果から実行用のバイナリ部分を受け取る
		IDxcBlob* shaderBlob = nullptr;
		hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
		assert(SUCCEEDED(hr));
		Log(WindowSetUp::debugLog, ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath_, profile_)));
		
		//もう使わないリソースを解放
		shaderSource->Release();
		shaderResult->Release();

		//実行用のバイナリをリターン
		return shaderBlob;
	}

	//実際にシェーダーをコンパイルする
	void CompileMyShaderes()
	{
		vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl",
			L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
		assert(vertexShaderBlob != nullptr);
		Log(WindowSetUp::debugLog, "VertexShader Compiled\n");

		pixcelShaderBlob = CompileShader(L"Object3D.PS.hlsl",
			L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
		assert(pixcelShaderBlob != nullptr);
		Log(WindowSetUp::debugLog, "PixcelShader Compiled\n");

	}

	void Finalize()
	{
		vertexShaderBlob->Release();
		pixcelShaderBlob->Release();
	}

};