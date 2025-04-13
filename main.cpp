#include <Windows.h>
#include "AboutDX.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	MyDX myDx;
	myDx.Initialize();



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
			//Dxの更新処理?
			myDx.Update();

			//ゲームの処理



		}

	}

	myDx.Finalize();

	return 0;
}