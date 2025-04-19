#pragma once

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_MAPTOOL, LEVEL_GAMEPLAY, LEVEL_BOSS_BAT, LEVEL_END };

	const unsigned int	g_iWinSizeX = 1815;
	const unsigned int	g_iWinSizeY = 1020;
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
using namespace Client;

//corvus       L:103  R:49
//villager m   L: 53  R : 29
//