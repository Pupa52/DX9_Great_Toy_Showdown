#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CActor{ pGraphic_Device }
{
}

CMonster::CMonster(const CMonster& Prototype)
	:CActor { Prototype }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CMonster::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CMonster::Update(_float fTimeDelta)
{
//#pragma region 빌보드
//	_float4x4		ViewMatrix{};
//
//	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
//
//	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
//
//	_float3		vScaled = m_pTransformCom->Get_Scaled();
//
//	for (size_t i = 0; i < CTransform::STATE_POSITION; i++)
//	{
//		if (i == CTransform::STATE_UP)
//			continue;
//		m_pTransformCom->Set_State(CTransform::STATE(i), *(_float3*)&ViewMatrix.m[i] * *((_float*)&vScaled + i));
//	}
//
//	/*m_pTransformCom->LookAt_ForLandObject(*(_float3*)&ViewMatrix.m[3]);*/
//#pragma endregion 빌보드
}

void CMonster::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Set_State()
{
}

HRESULT CMonster::Ready_Components()
{
	return S_OK;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonster::Clone(void* pArg)
{
	return nullptr;
}

void CMonster::Free()
{
	__super::Free();
}
