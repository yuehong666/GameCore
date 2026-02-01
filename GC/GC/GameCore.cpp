//------------------------------头文件包含部分-----------------------------------------------

#include<d3d9.h>
#include<d3dx9.h>
#include<tchar.h>

//----------------------------------库文件包含部分-------------------------------------------


#pragma comment(lib,"winmm.lib")//调用PlaySound函数所需的库文件



//-----------------------------------宏定义---------------------------------------------------


#define WINDOW_WIDTH 800              //窗口宽度
#define WINDOW_HEIGHT 600             //窗口高度
#define SCREEN_WIDTH 800              //视口宽度
#define SCREEN_HEIGHT 600             //视口高度
#define WINDOW_TITLE L"DX9学习实例"  //窗口标题
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }//定义一个安全释放宏，便于后面COM接口指针的释放

//---------------------------------------------------------------------------------------------
//顶点缓存使用四步曲之一:设计顶点格式
//---------------------------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x,y,z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE )//FVF灵活顶点格式

//--------------------------------全局变量声明-------------------------------------------------

LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;//Direct3D设备对象
ID3DXFont* g_pFont = NULL;//字体COM接口
float g_FPS = 0.0f;//帧速率
wchar_t g_strFPS[50];//包含帧速率的字符数组
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;//顶点缓冲区对象
LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;//索引缓存对象
LPD3DXMESH g_teapot = NULL;//茶壶对象
LPD3DXMESH g_cube = NULL;//盒子对象
LPD3DXMESH g_sphere = NULL;//球面体对象
LPD3DXMESH g_torus = NULL;//圆环对象
D3DXMATRIX g_WorldMatrix[4],R;//定义一些全局的世界矩阵

//--------------------------------全局函数声明-------------------------------------------------

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);//窗口过程函数
HRESULT Direct3D_Init(HWND hwnd);//这个函数进行Direct3D的初始化
HRESULT Objects_Init(HWND hwnd);//这个函数进行要绘制的物体的资源初始化
VOID Direct3D_Render(HWND hwnd);//这个函数进行Direct3D渲染代码的书写
VOID Direct3D_CleanUp();//这个函数进行清理COM资源以及其他资源
float GetFPS();//计算每秒帧速率
VOID Matrix_Set();//Direct3D四大变换

//------------------------------WinMain()函数--------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	//窗口创建四部曲之一：窗口类的设计
	WNDCLASSEX wndClass = { 0 };//用WNDCLASSEX定义一个窗口类
	wndClass.cbSize = sizeof( WNDCLASSEX );//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW ;//设置窗口样式
	wndClass.lpfnWndProc = WndProc;//设置指向窗口过程的指针
	wndClass.cbClsExtra = 0 ;//窗口类的附加内存
	wndClass.cbWndExtra = 0 ;//窗口的附加内存
	wndClass.hInstance = hInstance ;//指定包含窗口过程的程序的实例句柄
	wndClass.hIcon = (HICON)::LoadImage(NULL,L"icon.ico",IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_LOADFROMFILE);//本地加载自定义ico图标
	wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);//指定窗口类的光标句柄
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);//为hbrBackground成员指定一个白色画刷句柄
	wndClass.lpszMenuName = NULL;//用一个以空终止的字符串，指定菜单资源的名字
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";//指定窗口类的名字
	

	//窗口创建四部曲之二：注册窗口类

	if(!RegisterClassEx( &wndClass ))//注册窗口类
		return -1;

	//窗口创建四部曲之三：创建窗口

	HWND hwnd = CreateWindow(L"ForTheDreamOfGameDevelop",WINDOW_TITLE,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,WINDOW_WIDTH,WINDOW_HEIGHT,NULL,NULL,hInstance,NULL);


	//Direct3D资源的初始化
	Direct3D_Init(hwnd);
	
	//窗口创建四部曲之四：窗口的移动、显示与更新
	MoveWindow(hwnd,250,80,WINDOW_WIDTH,WINDOW_HEIGHT,true);//调整窗口显示的位置，位于左上角（250，80）处
	ShowWindow(hwnd,nShowCmd);//显示窗口
	UpdateWindow(hwnd);//窗口更新


	//消息循环
	MSG msg = { 0 };//初始化msg
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))//查看应用程序消息队列，有消息时将消息派发出去
		{
			TranslateMessage(&msg);//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);//该函数分发一个消息给窗口程序
		}
		else
		{
			Direct3D_Render(hwnd);//进行渲染
		}
	}
	//窗口类的注销
	UnregisterClass(L"ForTheDreamOfGameDevelop",wndClass.hInstance);//程序准备结束，注销窗口类

	return 0;
}
//------------------------------WndProc()函数---------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch( message )
	{
	case WM_PAINT://客户区重绘消息
		Direct3D_Render(hwnd);//渲染函数
		ValidateRect(hwnd,NULL);//更新客户区的显示
		break;
	case WM_KEYDOWN://键盘按下消息
		if(wParam == VK_ESCAPE)//如果按下键是ESC
			DestroyWindow(hwnd);//摧毁窗口，并发送一条WM_DESTROY消息
		break;
	case WM_DESTROY://若是窗口摧毁消息
		Direct3D_CleanUp();//清理资源
		PostQuitMessage(0);//线程终止请求
		break;
	default:
		return DefWindowProc(hwnd,message,wParam,lParam);//调用默认的窗口过程
	}
	return 0;
}

//---------------------------------Direct3D_Init()函数------------------------------------------------------

HRESULT Direct3D_Init(HWND hwnd)
{
	//Direct3D初始化四步曲之一：创接口
	LPDIRECT3D9 pD3D = NULL ;//Direct3D接口对象的创建
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )//初始化Direct3D接口对象，并进行DirectX版本协商
		return E_FAIL;

	//Direct3D初始化四步曲之二：取信息
	D3DCAPS9 caps; int vp = 0;
	if( FAILED( pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps ) ) )
	{
		return E_FAIL;
	}
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//Direct3D初始化四步曲之三：填内容
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//Direct3D初始化四步曲之四：创设备
	if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,vp,&d3dpp,&g_pd3dDevice ) ) )
		return E_FAIL;
	SAFE_RELEASE(pD3D)//释放接口对象

	if(!(S_OK==Objects_Init(hwnd)))return E_FAIL;//调用一次Objects_Init，进行渲染资源的初始化
	return S_OK;
}

//---------------------------------Objects_Init()函数------------------------------------------------------

HRESULT Objects_Init(HWND hwnd)
{
	//创建字体
	if( FAILED( D3DXCreateFont(g_pd3dDevice,36,0,0,1,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,DEFAULT_QUALITY,0,_T("微软雅黑"),&g_pFont) ) )
		return E_FAIL;


	//物体的创建
	if( FAILED( D3DXCreateBox( g_pd3dDevice,2,2,2,&g_cube,NULL ) ) )//立方体的创建
		return false;
	if( FAILED( D3DXCreateTeapot( g_pd3dDevice,&g_teapot,NULL ) ) )//茶壶的创建
		return false;
	if( FAILED( D3DXCreateSphere( g_pd3dDevice,1.5,25,25,&g_sphere,NULL ) ) )//球面体的创建
		return false;
	if( FAILED( D3DXCreateTorus( g_pd3dDevice,0.5f,1.2f,25,25,&g_torus,NULL ) ) )//圆环体的创建
		return false;

	//设置渲染状态
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);//关闭光照
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);//开启背面消隐
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);//设置线框填充模式

	return S_OK;
}

//---------------------------------Direct3D_Render()函数------------------------------------------------------

void Direct3D_Render(HWND hwnd)
{
	//渲染五步曲之一：清屏操作
	g_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);

	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(hwnd,&formatRect);

	//渲染五步曲之二：开始绘制
	g_pd3dDevice->BeginScene();
	//渲染五步曲之三：正式绘制
	//---------------------------------------------------------------------------------------------------------
	Matrix_Set();//调用四大变换函数
	//获取键盘消息并设置相应的填充模式
	if(::GetAsyncKeyState(0x31) & 0x8000f)//若数字键1被按下，进行线框填充
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	if(::GetAsyncKeyState(0x32) & 0x8000f)//若数字键2被按下，进行实体填充
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	//---------------------------------------------------------------------------------------------------------
	//绘制图形
	D3DXMatrixRotationY(&R,::timeGetTime()/1440.0f);//设置公转的矩阵

	//进行立方体的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[0],3.0f,-3.0f,0.0f);
	g_WorldMatrix[0] = g_WorldMatrix[0] * R;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&g_WorldMatrix[0]);
	g_cube->DrawSubset(0);
	//进行茶壶的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[1],-3.0f,-3.0f,0.0f);
	g_WorldMatrix[1] = g_WorldMatrix[1] * R;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&g_WorldMatrix[1]);
	g_teapot->DrawSubset(0);
	//进行圆环的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[2],3.0f,3.0f,0.0f);
	g_WorldMatrix[2] = g_WorldMatrix[2] * R;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&g_WorldMatrix[2]);
	g_torus->DrawSubset(0);
	//进行球面体的绘制
	D3DXMatrixTranslation(&g_WorldMatrix[3],-3.0f,3.0f,0.0f);
	g_WorldMatrix[3] = g_WorldMatrix[3] * R;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&g_WorldMatrix[3]);
	g_sphere->DrawSubset(0);
	//---------------------------------------------------------------------------------------------------------
	//在窗口右上角显示帧数
	int charCount = swprintf_s(g_strFPS,20,_T("FPS:%.0f"),GetFPS());
	g_pFont->DrawText(NULL,g_strFPS,charCount,&formatRect,DT_TOP | DT_LEFT,D3DCOLOR_XRGB(255,39,136) );

	

	//渲染五步曲之四：结束绘制
	g_pd3dDevice->EndScene();//结束绘制
	//渲染五步曲之五：显示翻转
	g_pd3dDevice->Present(NULL,NULL,NULL,NULL);//翻转与显示
}

//---------------------------------GetFPS()函数----------------------------------------------------------------

float GetFPS()
{
	//定义四个静态变量
	static float fps = 0;//需要的FPS值
	static int frameCount = 0;//帧数
	static float currentTime = 0.0f;//当前时间
	static float lastTime = 0.0f;//持续时间

	frameCount++;//每调用一次GetFPS()，就自增1
	currentTime = timeGetTime()*0.001f;//获取系统时间

	//如果大于1秒就进行一次FPS计算，同时frameCount清零

	if( currentTime - lastTime > 0.05f )//将时间控制在0.05秒
	{
		fps = (float)frameCount/( currentTime - lastTime );
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}

//---------------------------------Direct3D_CleanUp()函数------------------------------------------------------

void Direct3D_CleanUp()
{
	//释放COM接口对象
	SAFE_RELEASE(g_torus)
	SAFE_RELEASE(g_sphere)
	SAFE_RELEASE(g_cube)
	SAFE_RELEASE(g_teapot)
	SAFE_RELEASE(g_pIndexBuffer)
	SAFE_RELEASE(g_pVertexBuffer)
	SAFE_RELEASE(g_pFont)
	SAFE_RELEASE(g_pd3dDevice)
}

//-------------------------------Direct3D四大变换--------------------------------------------------------------

VOID Matrix_Set()
{
	//1.世界变化矩阵的设置
	D3DXMATRIX matWorld,Rx,Ry,Rz;
	D3DXMatrixIdentity(&matWorld);//单位化世界矩阵
	D3DXMatrixRotationX(&Rx,D3DX_PI*(::timeGetTime()/1000.0f));//绕X轴旋转
	D3DXMatrixRotationY(&Ry,D3DX_PI*(::timeGetTime()/1000.0f/2));//绕Y轴旋转
	D3DXMatrixRotationZ(&Rz,D3DX_PI*(::timeGetTime()/1000.0f/3));//绕Z轴旋转
	matWorld = Rx * Ry * Rz * matWorld;//最终的组合矩阵
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);//设置世界变换矩阵

	//2.取景变换矩阵的设置
	D3DXMATRIX matView;//定义一个矩阵
	D3DXVECTOR3 vEye(0.0f,0.0f,-20.0f);//摄像机的位置
	D3DXVECTOR3 vAt(0.0f,0.0f,0.0f);//观察点的的位置
	D3DXVECTOR3 vUp(0.0f,1.0f,0.0f);//向上的向量
	D3DXMatrixLookAtLH(&matView,&vEye,&vAt,&vUp);//计算出取景变化矩阵
	g_pd3dDevice->SetTransform(D3DTS_VIEW,&matView);//应用取景变化矩阵

	//3.投影变化矩阵的设置
	D3DXMATRIX matProj;//定义一个矩阵
	D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI/4.0f,1.0f,1.0f,1000.0f);//计算投影变换矩阵
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj);//设置投影变化矩阵

	//4.视口变换矩阵
	D3DVIEWPORT9 vp;
	vp.X = 0;//视口相对于窗口的X坐标
	vp.Y = 0;//视口相对于窗口的Y坐标
	vp.Width = SCREEN_WIDTH;//视口宽度
	vp.Height = SCREEN_HEIGHT;//视口高度
	vp.MinZ = 0.0f;//视口在深度缓存中的最小深度值
	vp.MaxZ = 1.0f;//视口在深度缓存中的最大深度值
	g_pd3dDevice->SetViewport(&vp);//视口的设置
}