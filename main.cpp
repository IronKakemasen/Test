#include <Windows.h>
#include "AboutDX.h"
#include "BaseClass.h"
#include "Camera.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//COMの初期化
	CoInitializeEx(0, COINITBASE_MULTITHREADED);

	MyDX myDx;
	myDx.Initialize();

	Camera camera({ 0.0f,0.0f,0.0f,1.0f });

	Transform trans;
	trans.scale = { 1.0f,1.0f,1.0f,1.0f };
	trans.pos = { 0.0f,0.0f,1.0f,1.0f };

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
			camera.trans.mat = Get_SRTMat3D(camera.trans.scale, camera.trans.rotateTheta, 
				camera.trans.pos);
			//VP
			Mat4 vpvmat = Get_VPMat(camera.trans.pos, camera.trans.mat);

			//W
			trans.mat = Get_SRTMat3D(trans.scale, trans.rotateTheta, trans.pos);
			trans.rotateTheta.y -= 0.5f;

			//WVP
			Mat4 newMat = trans.mat.Multiply(vpvmat);
			
			*myDx.iD3D12SetUp.wvpData = newMat;


		}

	}

	myDx.Finalize();
	//COMの終了
	CoUninitialize();

	return 0;
}