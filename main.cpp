#include<Windows.h>
#include<time.h>
//--------------------------全局变量------------------------------------------------------------------------

HDC g_hdc = NULL;//全局设备环境句柄
HPEN g_hPen[7] = {0};//定义画笔句柄的数组
HBRUSH g_hBrush[7] = {0};//定义画刷句柄的数组
int g_iPenStyle[7] = {PS_SOLID,PS_DASH,PS_DOT,PS_DASHDOT,PS_DASHDOTDOT,PS_NULL,PS_INSIDEFRAME};//定义画笔样式数组并初始化
int g_iBrushStyle[6] = {HS_VERTICAL,HS_HORIZONTAL,HS_CROSS,HS_DIAGCROSS,HS_FDIAGONAL,HS_BDIAGONAL};//定义画刷样式数组并初始化

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
	srand((unsigned)time(NULL));//初始化时间种子

	//随机初始化画笔和画刷的颜色值
	for(int i=0;i<=6;i++)
	{
		g_hPen[i] = CreatePen(g_iPenStyle[i],1,RGB(rand()%256,rand()%256,rand()%256));
		if(i==6)
			g_hBrush[i] = CreateSolidBrush(RGB(rand()%256,rand()%256,rand()%256));
		else
			g_hBrush[i] = CreateHatchBrush(g_iBrushStyle[i],RGB(rand()%256,rand()%256,rand()%256));
	}



	Game_Paint(hwnd);
	ReleaseDC(hwnd,g_hdc);
	return TRUE;
}

//Game_Paint()绘画函数

VOID Game_Paint(HWND hwnd)
{
	//定义一个Y坐标值
	int y=0;

	//用7种不同的画笔绘制线条
	for(int i=0;i<=6;i++)
	{
		y=(i+1)*70;

		SelectObject(g_hdc,g_hPen[i]);//将对应的画笔选好
		MoveToEx(g_hdc,30,y,NULL);//从光标移动到（30，y）
		LineTo(g_hdc,100,y);//从（30，y）向（100，y）绘制线段
	}
	//注意上面画完后Y=420，下面画矩形的时候还有用
	//定义两个X坐标值
	int x1=120;
	int x2=190;

	//用7种不同的画刷填充矩形
	for(int i=0;i<=6;i++)
	{
		SelectObject(g_hdc,g_hBrush[i]);//选择画刷
		Rectangle(g_hdc,x1,70,x2,y);//画一个矩形（x1,50）(x2,y)
		x1+=90;
		x2+=90;
	}
}

//Game_Cleanup()资源清理函数

BOOL Game_Cleanup(HWND hwnd)
{
	//释放所有画笔画刷句柄
	for(int i=0;i<=6;i++)
	{
		DeleteObject(g_hPen[i]);
		DeleteObject(g_hBrush[i]);
	}
	return TRUE;
}