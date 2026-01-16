#include<Windows.h>
#include<time.h>
//--------------------------全局变量------------------------------------------------------------------------

HDC g_hdc = NULL,g_mdc=NULL;//全局设备环境句柄
HBITMAP g_hBackGround=NULL,g_001=NULL,g_002=NULL;//定义位图句柄

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
	Game_Cleanup(hwnd);
	UnregisterClass(L"ForTheDreamOfGameDevelop",wndClass.hInstance);
	


	return 0;
}

//窗口过程函数

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT paintStruct;//定义一个PAINTSTRUCT结构体记录绘制消息


	switch(message)
	{
	case WM_PAINT://若是客户区重绘消息
		g_hdc=BeginPaint(hwnd,&paintStruct);//指定窗口进行绘制，将绘制有关的消息填入PAINTSTRUCT结构体中
		Game_Paint(hwnd);
		EndPaint(hwnd,&paintStruct);//绘画结束
		ValidateRect(hwnd,NULL);//更新客户区的显示
		break;
	case WM_KEYDOWN://键盘按下消息
		if(wParam == VK_ESCAPE)//如果是ESC建
			DestroyWindow(hwnd);//销毁窗口,并发送WM_DESTROY消息
		break;
	case WM_DESTROY://窗口销毁消息
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
	
	//----------位图绘制四步曲之一：加载位图-------------

	g_hBackGround = (HBITMAP)LoadImage(NULL,L"lol.bmp",IMAGE_BITMAP,800,600,LR_LOADFROMFILE);//加载背景位图
	g_001 = (HBITMAP)LoadImage(NULL,L"001.bmp",IMAGE_BITMAP,800,600,LR_LOADFROMFILE);//加载001图片位图
	g_002 = (HBITMAP)LoadImage(NULL,L"002.bmp",IMAGE_BITMAP,800,600,LR_LOADFROMFILE);//加载002图片位图

	//-----------位图绘制四步曲之二：建立兼容DC----------

	g_mdc = CreateCompatibleDC(g_hdc);//建立兼容设备环境的内存DC



	Game_Paint(hwnd);
	ReleaseDC(hwnd,g_hdc);//释放设备环境
	return TRUE;
}

//Game_Paint()绘画函数

VOID Game_Paint(HWND hwnd)
{
	//先贴背景图
	SelectObject(g_mdc,g_hBackGround);
	BitBlt(g_hdc,0,0,800,600,g_mdc,0,0,SRCCOPY);
	//透明遮罩法
	SelectObject(g_mdc,g_002);
	BitBlt(g_hdc,0,0,800,600,g_mdc,0,0,SRCAND);
	SelectObject(g_mdc,g_001);
	BitBlt(g_hdc,0,0,800,600,g_mdc,0,0,SRCPAINT);

}

//Game_Cleanup()资源清理函数

BOOL Game_Cleanup(HWND hwnd)
{
	//释放资源对象
	DeleteObject(g_hBackGround);
	DeleteObject(g_001);
	DeleteObject(g_002);
	DeleteDC(g_mdc);
	return TRUE;
}