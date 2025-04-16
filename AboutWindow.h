#pragma once
#include <Windows.h>
#include <cstdint>
#include <cassert>
#include "AboutDebugLog.h"
#include "AboutException.h"
#include "AboutImgui.h"
#include "VertexData.h"

//ウィンドウプロシージャ
inline LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
	WPARAM wparam, LPARAM lparam)
{
	//ImGuiにメッセージを渡す
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}

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

struct WindowSetUp
{
	//デバッグログ格納用
	inline static std::ofstream debugLog;
	//クライアントの領域サイズ
	int32_t kClientWidth;
	int32_t kClientHeight;
	//ウィンドウハンドル
	HWND hwnd;

	void SetWindowAndSome(int32_t kWindowWidth_, int32_t kWindowHeight_)
	{
		WNDCLASS wc{};
		//ウィンドウプロシージャ(複数の処理を一つにまとめたもの)
		wc.lpfnWndProc = WindowProc;
		//ウィンドウクラス名
		wc.lpszClassName = L"title";
		//インスタンスハンドル
		wc.hInstance = GetModuleHandle(nullptr);
		//カーソル
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		//ウィンドウクラスを登録する
		RegisterClass(&wc);
		//クライアントの領域サイズ
		kClientWidth = kWindowWidth_;
		kClientHeight = kWindowHeight_;

		//ウィンドウサイズを表す構造体にクライアント領域を入れる
		RECT wrc = { 0,0,kClientWidth,kClientHeight };
		//クライアント領域をもとに実際のサイズにwrcを変更してもらう
		AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

		//[ ウィンドウの生成 ]
		hwnd = CreateWindow(
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

		//[ ウィンドウを表示する ]
		ShowWindow(hwnd, SW_SHOW);

		//[ Log ]
		std::filesystem::create_directory("DebugLog");
		debugLog = DebugLogInitialize();

		//[ デバッグヘルパー ]
		//誰も捕捉しなかった場合に(Unhandled)、補足する関数を登録
		//main関数始まってすぐに登録するといい
		SetUnhandledExceptionFilter(ExportDump);

	}

};

