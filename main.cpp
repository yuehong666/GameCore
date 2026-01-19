#include<iostream>
#include<Windows.h>
#include<time.h>
#include<tchar.h>
#include<wchar.h>
//--------------------------全局变量------------------------------------------------------------------------

HDC g_hdc = NULL,g_mdc=NULL;//全局设备环境句柄
HBITMAP g_hSprite[12];//声明位图数组来储存各张位图
int g_iNum=0;//用于记录目前显示的图号

//--------------------------声明函数------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);//窗口过程函数
BOOL Game_Init(HWND hwnd);//资源初始化函数
VOID Game_Paint(HWND hwnd);//绘画函数
BOOL Game_Cleanup(HWND hwnd);//资源清理函数

//--------------------------WinMain()函数-------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdline,int nShowCmd)
{
	//第一步：窗口设计

	WNDCLASSEX wndClass = { 0 };//定义一个窗口类
	wndClass.cbSize = sizeof( WNDCLASSEX ); //设置结构体的字节数大小
	wndClass.style =CS_HREDRAW | CS_VREDRAW;//设置窗口样式
	wndClass.lpfnWndProc = WndProc;//设置指向窗口过程函数的指针
	wndClass.cbClsExtra = 0;//窗口类的附加内存
	wndClass.cbWndExtra = 0;//窗口的附加内存
	wndClass.hInstance = hInstance;//指定包含窗口过程的程序的实例句柄
	wndClass.hIcon = (HICON)::LoadImage(NULL,L"icon.ico",IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_LOADFROMFILE);//本地加载自定义ioc图标
	wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);//指定窗口类的光标句柄
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);//为hbrBackground成员指定一个灰色画刷句柄
	wndClass.lpszMenuName = NULL;//用一个以空终止的字符串，指定菜单资源的名字
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";//用一个以空终止的字符串，指定窗口类的名字

	//第二步窗口注册

	if(!RegisterClassEx(&wndClass))
		return -1;

	//第三步窗口创建

	HWND hwnd = CreateWindow(L"ForTheDreamOfGameDevelop",L"为了正义",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,800,600,NULL,NULL,hInstance,NULL);

	//第四步窗口的更新

	MoveWindow(hwnd,250,80,800,600,true);//调整窗口显示时的位置
	ShowWindow(hwnd,nShowCmd);//显示窗口
	UpdateWindow(hwnd);//更新窗口
	//游戏资源初始化
	if(!Game_Init(hwnd))
	{
		MessageBox(hwnd,L"初始化失败！",L"消息窗口",0);
		return FALSE;
	}
	//第五步消息循环

	MSG msg = { 0 };
	while(msg.message != WM_QUIT)
	{
		if( PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);//将虚拟建消息转换为字符消息
			DispatchMessage(&msg);//分发一个消息给窗口程序
		}
	}

	//第六步窗口类的注销
	UnregisterClass(L"ForTheDreamOfGameDevelop",wndClass.hInstance);
	


	return 0;
}

//窗口过程函数

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT paintStruct;//定义一个PAINTSTRUCT结构体记录绘制消息


	switch(message)
	{
	case WM_TIMER://定时器消息
		Game_Paint(hwnd);//绘图函数
		break;
	case WM_KEYDOWN://键盘按下消息
		if(wParam == VK_ESCAPE)//如果是ESC建
			DestroyWindow(hwnd);//销毁窗口,并发送WM_DESTROY消息
		break;
	case WM_DESTROY://窗口销毁消息

		Game_Cleanup(hwnd);//资源清理

		PostQuitMessage(0);//终止线程请求

		break;
	default:
		return DefWindowProc(hwnd,message,wParam,lParam);//默认窗口过程
	}
	return 0;
}

//Game_Init()资源初始化函数

BOOL Game_Init(HWND hwnd)
{
	g_hdc = GetDC(hwnd);//获取设备环境句柄
	
	wchar_t filename[20];
	//载入各个位图
	for(int i=0;i<12;i++)
	{
		memset(filename,0,sizeof(filename));//filename初始化
		swprintf_s(filename,L"%d.bmp",i);//组装对应的图片名称
		g_hSprite[i] = (HBITMAP)LoadImage(NULL,filename,IMAGE_BITMAP,800,600,LR_LOADFROMFILE);//加载位图
	}
	//-----------位图绘制四步曲之二：建立兼容DC----------

	g_mdc = CreateCompatibleDC(g_hdc);//建立兼容设备环境的内存DC

	g_iNum = 0;//设置初始的图号
	SetTimer(hwnd,1,90,NULL);//建立定时器，间隔0.09秒

	return TRUE;
}

//Game_Paint()绘画函数

VOID Game_Paint(HWND hwnd)
{
	//处理图号
	if(g_iNum == 11)
		g_iNum = 0;

	//依据图号来贴图
	SelectObject(g_mdc,g_hSprite[g_iNum]);//根据图号选择位图
	BitBlt(g_hdc,0,0,800,600,g_mdc,0,0,SRCCOPY);//进行贴图
	g_iNum++;
}

//Game_Cleanup()资源清理函数

BOOL Game_Cleanup(HWND hwnd)
{
	KillTimer(hwnd,1);//删除定时器
	//释放资源对象
	for(int i=0;i<12;i++)
		DeleteObject(g_hSprite[i]);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd,g_hdc);
	return TRUE;
}