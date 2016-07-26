#include "stdafx.h"
#include "resource.h"
#include <Windows.h>
#include <strsafe.h>
#include <gl\GL.h>
#include "glext\wglext.h"
#include "glext\glext.h"
#include "glext\glcorearb.h"
#include "wnd.h"


#pragma comment(lib, "opengl32.lib")

void StartWindow()
{
	HINSTANCE h = GetModuleHandle(NULL);
	WinMain(h, 0, NULL, SW_SHOW);
}

void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	//ExitProcess(dw);
}


LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static CREATESTRUCT s;
	switch (message)
	{
	case WM_CREATE:
		s = *(CREATESTRUCT*)lParam;
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
			//case IDM_ABOUT:
			//	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		//_stprintf_s(text, _T("%8.8x\0"), *id);

		//RECT rc;
		//rc.left = 0;
		//rc.top = 0;
		//rc.right = 100;
		//rc.bottom = 100;

		//DrawText(hdc, text, _tcslen(text), &rc, DT_CENTER | DT_VCENTER);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW |
		CS_VREDRAW;                    // redraw if size changes 
	wcx.lpfnWndProc = MainWndProc;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = hInstance;         // handle to instance 
	wcx.hIcon = LoadIcon(NULL,
		IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL,
		IDC_ARROW);                    // predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(
		WHITE_BRUSH);                  // white background brush 
	wcx.lpszMenuName = _T("MainMenu");    // name of menu resource 
	wcx.lpszClassName = _T("MainWClass");  // name of window class 
	wcx.hIconSm = (HICON)NULL;
	/*
	LoadImage(hInstance, // small class icon
	MAKEINTRESOURCE(5),
	IMAGE_ICON,
	GetSystemMetrics(SM_CXSMICON),
	GetSystemMetrics(SM_CYSMICON),
	LR_DEFAULTCOLOR);
	*/
	// Register the window class. 

	return RegisterClassEx(&wcx);
}

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hwnd;

	// Save the application-instance handle. 

	//hinst = hInstance;

	// Create the main window. 

	hwnd = CreateWindow(
		//0,
		_T("MainWClass"),        // name of window class 
		_T("Sample"),            // title-bar string 
		WS_OVERLAPPEDWINDOW, // top-level window 
		CW_USEDEFAULT,       // default horizontal position 
		CW_USEDEFAULT,       // default vertical position 
		CW_USEDEFAULT,       // default width 
		CW_USEDEFAULT,       // default height 
		(HWND)NULL,         // no owner window 
		(HMENU)NULL,        // use class menu 
		hInstance,           // handle to application instance 
		(LPVOID)NULL);      // no window-creation data 

	if (!hwnd)
		return FALSE;


	HDC dc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
		1,                     // version number  
		PFD_DRAW_TO_WINDOW |   // support window  
		PFD_SUPPORT_OPENGL |   // support OpenGL  
		PFD_DOUBLEBUFFER,      // double buffered  
		PFD_TYPE_RGBA,         // RGBA type  
		32,                    // 32-bit color depth  
		0, 0, 0, 0, 0, 0,      // color bits ignored  
		0,                     // no alpha buffer  
		0,                     // shift bit ignored  
		0,                     // no accumulation buffer  
		0, 0, 0, 0,            // accum bits ignored  
		24,                    // 24-bit z-buffer  
		8,                     // 8-bit stencil buffer  
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored  
	};

	int  iPixelFormat;
	// get the best available match of pixel format for the device context   
	iPixelFormat = ChoosePixelFormat(dc, &pfd);

	// make that the pixel format of the device context  
	SetPixelFormat(dc, iPixelFormat, &pfd);

	HGLRC shared_rc = wglCreateContext(dc);
	//ErrorExit(__FUNCTIONW__);
	wglMakeCurrent(dc, shared_rc);
	char* str = (char*)glGetString(GL_EXTENSIONS);

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

	char* ext = (char*)wglGetExtensionsStringARB(dc);

	const int attribList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 5,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0,        //End
	};

	int pixelFormat;
	UINT numFormats;

	wglChoosePixelFormatARB(dc, attribList, NULL, 1, &pixelFormat, &numFormats);
	HGLRC rc = wglCreateContextAttribsARB(dc, shared_rc, attribList);
	wglMakeCurrent(dc, rc);

	// Show the window and send a WM_PAINT message to the window 
	// procedure. 
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);


	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	if (!InitApplication(hInstance))
		return FALSE;

	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACC));

	memset(&msg, 0, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE) &&
			!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}

static struct GLAPP *app = NULL;
struct GLAPP * GetApp()
{
	if (app == NULL) {
		app = (struct GLAPP*)malloc(sizeof(struct GLAPP));
		app->GLInit = &StartWindow;
		return app;
	}
	return NULL;
}
