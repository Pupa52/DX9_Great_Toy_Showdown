#include "stdafx.h"
#include "Actor.h"

CActor::CActor(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{
}

CActor::CActor(const CActor& Prototype)
	: CLandObject ( Prototype )
	, m_tActorDesc { Prototype.m_tActorDesc }
{
}

HRESULT CActor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CActor::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

_uint CActor::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CActor::Update(_float fTimeDelta)
{
}

void CActor::Late_Update(_float fTimeDelta)
{
}

HRESULT CActor::Render()
{
	return S_OK;
}

void CActor::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCubeCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pAnimatorCom);
}
