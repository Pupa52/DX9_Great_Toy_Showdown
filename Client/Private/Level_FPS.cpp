#include "stdafx.h"
#include "..\Public\Level_FPS.h"

#include "Level_Loading.h"

#include "GameInstance.h"

CLevel_FPS::CLevel_FPS(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }
{
}

HRESULT CLevel_FPS::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	return S_OK;
}

void CLevel_FPS::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_GAMEPLAY
			, PLAYER_CHARACTER::PLAYER_CAT))))
			return;
	}
}

HRESULT CLevel_FPS::Render()
{
	SetWindowText(g_hWnd, TEXT("FPS 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_FPS::Ready_Layer_BackGround()
{
	return S_OK;
}

CLevel_FPS* CLevel_FPS::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_FPS* pInstance = new CLevel_FPS(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_FPS"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_FPS::Free()
{
	__super::Free();
}
