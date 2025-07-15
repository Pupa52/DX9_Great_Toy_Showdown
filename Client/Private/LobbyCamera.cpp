#include "stdafx.h"
#include "..\Public\LobbyCamera.h"
#include "Level_Loddy.h"

CLobbyCamera::CLobbyCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CLobbyCamera::CLobbyCamera(const CLobbyCamera& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CLobbyCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobbyCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vEye);
	m_pTransformCom->LookAt(pDesc->vAt);

	m_fFovy = pDesc->fFovy;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	m_pCatTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component
	(LEVEL_LOBBY, TEXT("Layer_LobbyPlayer"), TEXT("Com_Transform"), 0));
	Safe_AddRef(m_pCatTransform);

	m_pDogTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component
	(LEVEL_LOBBY, TEXT("Layer_LobbyPlayer"), TEXT("Com_Transform"), 1));
	Safe_AddRef(m_pDogTransform);

	m_pPenguinTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component
	(LEVEL_LOBBY, TEXT("Layer_LobbyPlayer"), TEXT("Com_Transform"), 2));
	Safe_AddRef(m_pPenguinTransform);

	// 캐릭터, 카메라 거리
	m_vCameraDist = { 0.f, 2.f, -10.f };
	m_vLookCharacterPos = m_pCatTransform->Get_State(CTransform::STATE_POSITION) + m_vCameraDist;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vLookCharacterPos);
	m_pTransformCom->LookAt(m_pCatTransform->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

_uint CLobbyCamera::Priority_Update(_float fTimeDelta)
{
	Camera_Control();

	PLAYER_CHARACTER ePlayerCharacter = static_cast<CLevel_Loddy*>(m_pGameInstance->Get_CurrentLevel())->Get_PlayerCharacter();
	_float3 vLookPos = m_pCatTransform->Get_State(CTransform::STATE_POSITION);
	switch (ePlayerCharacter)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		vLookPos = m_pCatTransform->Get_State(CTransform::STATE_POSITION);
		m_vLookCharacterPos = m_pCatTransform->Get_State(CTransform::STATE_POSITION) + m_vCameraDist;
		break;

	case PLAYER_CHARACTER::PLAYER_DOG:
		vLookPos = m_pDogTransform->Get_State(CTransform::STATE_POSITION);
		m_vLookCharacterPos = m_pDogTransform->Get_State(CTransform::STATE_POSITION) + m_vCameraDist;
		break;

	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		vLookPos = m_pPenguinTransform->Get_State(CTransform::STATE_POSITION);
		m_vLookCharacterPos = m_pPenguinTransform->Get_State(CTransform::STATE_POSITION) + m_vCameraDist;
		break;

	default:
		break;
	}
	MoveTo_Character(m_vLookCharacterPos, vLookPos, fTimeDelta);

	/* 장치에 뷰랑 투영행렬을 한번 셋팅 해주면 다시 갱신해 주기 전까지 모든 객체들을 렌더링시에 셋팅해준 행렬을 곱하면서 그리게 된다. */
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_pTransformCom->Get_WorldMatrix_Inverse());
	_float4x4		ProjMatrix{};
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&ProjMatrix, m_fFovy, (_float)g_iWinSizeX / g_iWinSizeY, m_fNear, m_fFar));

	return OBJ_NOEVENT;
}

void CLobbyCamera::Update(_float fTimeDelta)
{

}

void CLobbyCamera::Late_Update(_float fTimeDelta)
{

}

HRESULT CLobbyCamera::Render()
{
	return S_OK;
}

void CLobbyCamera::Camera_Control()
{
	if (m_pGameInstance->Key_Pressing('Z'))
	{
		m_fFovy -= 0.01f;
	}
	else if (m_pGameInstance->Key_Pressing('X'))
	{
		m_fFovy += 0.01f;
	}

	if (m_pGameInstance->Key_Down('C'))
	{
		m_fFovy = D3DXToRadian(60.f);
	}
}

HRESULT CLobbyCamera::Ready_Components(void* pArg)
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), pArg)))
		return E_FAIL;

	return S_OK;
}

void CLobbyCamera::MoveTo_Character(_float3 vPos, _float3 vLookPos, _float fTimeDelta)
{
	_float3 vDir = vPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (D3DXVec3Length(&vDir) < 0.5f)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom->LookAt(vLookPos);
		return;
	}

	D3DXVec3Normalize(&vDir, &vDir);

	_float3 vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vCameraPos += vDir * fTimeDelta * 10.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos);

}

CLobbyCamera* CLobbyCamera::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLobbyCamera* pInstance = new CLobbyCamera(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobbyCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CLobbyCamera::Clone(void* pArg)
{
	CLobbyCamera* pInstance = new CLobbyCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLobbyCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobbyCamera::Free()
{
	__super::Free();

	Safe_Release(m_pCatTransform);
	Safe_Release(m_pDogTransform);
	Safe_Release(m_pPenguinTransform);

	Safe_Release(m_pTransformCom);
}
