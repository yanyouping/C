// glc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <gl\GL.h>
#include <process.h>
#include "resource.h"
#include "glext\wglext.h"
#include "glext\glext.h"
#include "glext\glcorearb.h"
#include "wnd.h"

TCHAR text[256];



int main()
{
	struct GLAPP *glapp = GetApp();
	glapp->GLInit();
	system("pause");
}

