#include "stdafx.h"
#include "Part.h"

CPart::CPart(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(pGraphic_Device);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPart::Initialize(_uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID)
{
	if (nullptr == pArg)
		return E_FAIL;

	// 플레이어 위치 받아오기
	PART_DESC* desc = static_cast<PART_DESC*>(pArg);

	m_pTransformCom = desc->pTransformCom;
	Safe_AddRef(m_pTransformCom);

	m_pVIBufferCom = desc->pVIBufferCom;
	Safe_AddRef(m_pVIBufferCom);

	m_ePlayerID = ePlayerID;

	return S_OK;
}

void CPart::Priority_Update(_float fTimeDelta)
{
	m_pTransformPart->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
	m_pTransformPart->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_fDist);
	
	_float fAngle = D3DXToDegree(m_fAngle);

	if (fAngle >= 0.f && fAngle < 90.f)
		m_eCurDir = PLAYER_DIR::RIGHTTOP;
	else if (fAngle >= 90.f && fAngle <= 180.f)
		m_eCurDir = PLAYER_DIR::LEFTTOP;
	else if (fAngle >= -90.f && fAngle < 0.f)
		m_eCurDir = PLAYER_DIR::RIGHTDOWN;
	else if (fAngle >= -180.f && fAngle < -90.f)
		m_eCurDir = PLAYER_DIR::LEFTDOWN;
}

void CPart::Update(_float fTimeDelta)
{
}

void CPart::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Move_Frame(fTimeDelta * 2.f);
}

HRESULT CPart::Render()
{

	return S_OK;
}

_bool CPart::Dir_Changed()
{
	if (m_eCurDir != m_ePreDir)
	{
		m_ePreDir = m_eCurDir;
		return true;
	}

	return false;
}

HRESULT CPart::Ready_Components(_uint iLevelIndex)
{
	m_pAnimatorCom = static_cast<CAnimator*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		TEXT("Prototype_Component_Animator"), nullptr));

	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	/* For.Com_Transform */
	m_pTransformPart = static_cast<CTransform*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		TEXT("Prototype_Component_Transform"), &TransformDesc));

	return S_OK;
}


void CPart::Inverse_Vertex()
{
	if (m_eCurDir == PLAYER_DIR::LEFTDOWN || m_eCurDir == PLAYER_DIR::LEFTTOP)
		m_bInverse = true;
	else
		m_bInverse = false;
}

HRESULT CPart::SetUp_RenderState()
{
	if (true == m_bInverse)
		m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	else
		m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

HRESULT CPart::Reset_RenderState()
{
	if (true == m_bInverse)
		m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

void CPart::Free()
{
	__super::Free();
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pTransformPart);
}
