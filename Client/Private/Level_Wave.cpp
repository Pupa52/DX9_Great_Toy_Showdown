#include "stdafx.h"
#include "..\Public\Level_Wave.h"

#include "Level_Loading.h"

#include "GameInstance.h"

CLevel_Wave::CLevel_Wave(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }
{
}

HRESULT CLevel_Wave::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Wave::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_GAMEPLAY
			, PLAYER_CHARACTER::PLAYER_CAT))))
			return;
	}
}

HRESULT CLevel_Wave::Render()
{
	SetWindowText(g_hWnd, TEXT("Wave 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Wave::Ready_Layer_BackGround()
{
	return S_OK;
}

CLevel_Wave* CLevel_Wave::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Wave* pInstance = new CLevel_Wave(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Wave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Wave::Free()
{
	__super::Free();
}
