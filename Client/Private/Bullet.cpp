#include "stdafx.h"
#include "..\Public\Bullet.h"

#include "GameInstance.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CBullet::CBullet(const CBullet& Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	return S_OK;
}

_uint CBullet::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CBullet::Update(_float fTimeDelta)
{
}

void CBullet::Late_Update(_float fTimeDelta)
{
}

HRESULT CBullet::Render()
{
	return S_OK;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBullet::Clone(void* pArg)
{
	return S_OK;
}

void CBullet::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pColliderCubeCom);
}
