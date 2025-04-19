#pragma once

namespace ToolModel
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	/* 클라이언트에서 사용하기위한 여러 정의들을 모아둘꺼야. */
	const unsigned int	g_iWinSizeX = 1280;
	const unsigned int	g_iWinSizeY = 720;
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
using namespace ToolModel;