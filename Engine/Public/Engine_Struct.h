#pragma once

namespace Engine
{
	typedef struct tagEngine_Desc
	{
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		unsigned int	iMaxSoundChannel;
		bool			isWindowsed;
	}ENGINE_DESC;

	

	typedef struct 
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR2		vTexcoord;
	}VTXTEX;

	typedef struct
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vNormal;
		D3DXVECTOR2		vTexcoord;
	}VTXNORTEX;

	typedef struct
	{
		D3DXVECTOR3		vPosition;		
		D3DXVECTOR3		vTexcoord;
	}VTXCUBE;

}