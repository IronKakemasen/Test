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
	trans.pos = { -0.25f,0.0f,2.5f,1.0f };

	Transform trans2;
	trans2.scale = { 1.0f,1.0f,1.0f,1.0f };
	trans2.pos = { 0.0f,0.0f,5.0f,1.0f };



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
			Matrix4 vpvmat = Get_VPMat(camera.trans.pos, camera.trans.mat);

			//W
			trans.mat = Get_SRTMat3D(trans.scale, trans.rotateTheta, trans.pos);
			trans.rotateTheta.y -= 0.5f;
			trans.rotateTheta.x -= 0.5f;

			//WVP
			Matrix4 newMat = trans.mat.Multiply(vpvmat);
			
			myDx.iD3D12SetUp.transformationMatrixData->WVP = newMat;
			myDx.iD3D12SetUp.transformationMatrixData->World = trans.mat;




			Matrix4 inv_CameraMat = camera.trans.mat.GetInversed();
			Matrix4 projectionMat = Get_Orthographic3D(0.0f, 1280.0f, 0.0f, 720.0f, 0.0f, 100.0f);
			trans2.mat = Get_SRTMat3D(trans2.scale, trans2.rotateTheta, trans2.pos);

			Matrix4 vpMat2 = inv_CameraMat.Multiply(projectionMat);
			Matrix4 wvpMat2 = trans2.mat.Multiply(vpMat2);

			myDx.iD3D12SetUp.transformationMatrixSpriteData->WVP = wvpMat2;





		}

	}

	myDx.Finalize();
	//COMの終了
	CoUninitialize();

	return 0;
}