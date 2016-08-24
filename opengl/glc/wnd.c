#include "stdafx.h"
#include "resource.h"
#include <Windows.h>
#include <strsafe.h>
#include <gl\GL.h>
#include "glext\wglext.h"
#include "glext\glext.h"
#include "glext\glcorearb.h"
#include "wnd.h"
#include <varargs.h>


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

static int wsize[2];

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
	case WM_ERASEBKGND:
		return TRUE;
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
	case WM_SIZE:
		wsize[0] = LOWORD(lParam);
		wsize[1] = HIWORD(lParam);
		return 0;
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

	wcx.cbSize = sizeof(wcx);						// size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;			// redraw if size changes 
	wcx.lpfnWndProc = MainWndProc;					// points to window procedure 
	wcx.cbClsExtra = 0;								// no extra class memory 
	wcx.cbWndExtra = 0;								// no extra window memory 
	wcx.hInstance = hInstance;						// handle to instance 
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);		// predefined arrow 
	wcx.hbrBackground = NULL;						// (HBRUSH)GetStockObject(WHITE_BRUSH); // white background brush 
	wcx.lpszMenuName = _T("MainMenu");				// name of menu resource 
	wcx.lpszClassName = _T("MainWClass");			// name of window class 
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

PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog;
PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup;
PFNGLPOPDEBUGGROUPPROC glPopDebugGroup;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
PFNGLCLEARBUFFERFVPROC glClearBufferfv;

void APIENTRY DebugProc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	char debSource[32], debType[32], debSev[32];

	if (source == GL_DEBUG_SOURCE_API_ARB)
		strcpy_s(debSource, 32, "OpenGL");
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
		strcpy_s(debSource, 32, "Windows");
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
		strcpy_s(debSource, 32, "Shader Compiler");
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
		strcpy_s(debSource, 32, "Third Party");
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
		strcpy_s(debSource, 32, "Application");
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
		strcpy_s(debSource, 32, "Other");
	else
		return;

	if (type == GL_DEBUG_TYPE_ERROR)
		strcpy_s(debType, 32, "error");
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
		strcpy_s(debType, 32, "deprecated behavior");
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
		strcpy_s(debType, 32, "undefined behavior");
	else if (type == GL_DEBUG_TYPE_PORTABILITY)
		strcpy_s(debType, 32, "portability");
	else if (type == GL_DEBUG_TYPE_PERFORMANCE)
		strcpy_s(debType, 32, "performance");
	else if (type == GL_DEBUG_TYPE_OTHER)
		strcpy_s(debType, 32, "message");
	else if (type == GL_DEBUG_TYPE_MARKER)
		strcpy_s(debType, 32, "marker");
	else if (type == GL_DEBUG_TYPE_PUSH_GROUP)
		strcpy_s(debType, 32, "push group");
	else if (type == GL_DEBUG_TYPE_POP_GROUP)
		strcpy_s(debType, 32, "pop group");
	else
		return;

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
	{
		strcpy_s(debSev, 32, "high");
		//if (Test->Success == GENERATE_ERROR || source != GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
		//	Test->Error = true;
	}
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
		strcpy_s(debSev, 32, "medium");
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)
		strcpy_s(debSev, 32, "low");
	else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		strcpy_s(debSev, 32, "notification");
	else
		return;

	fprintf_s(stderr, "%s: %s(%s) %d: %s\n", debSource, debType, debSev, id, message);

	//if (Test->Success != GENERATE_ERROR && source != GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
	//	assert(!Test->Error);
}

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

	PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)wglGetProcAddress("glDebugMessageControl");
	PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)wglGetProcAddress("glDebugMessageInsert");
	PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");
	PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC)wglGetProcAddress("glGetDebugMessageLog");
	PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)wglGetProcAddress("glPushDebugGroup");
	PFNGLPOPDEBUGGROUPPROC glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)wglGetProcAddress("glPopDebugGroup");

	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)wglGetProcAddress("glClearBufferfv");

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

#if defined(_DEBUG) && defined(GL_KHR_debug)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		glDebugMessageCallback(&DebugProc, 0);
	}
#endif


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

	GLfloat clr[4] = { 0,0,.7f,1 };
	memset(&msg, 0, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE) &&
			!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		glViewport(0, 0, wsize[0], wsize[1]);
		glClearBufferfv(GL_COLOR, 0, clr);
		SwapBuffers(wglGetCurrentDC());
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
