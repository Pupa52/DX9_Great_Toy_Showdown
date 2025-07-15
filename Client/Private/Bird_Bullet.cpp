#include "stdafx.h"
#include "..\Public\Bird_Bullet.h"

#include "Player.h"

CBird_Bullet::CBird_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet{ pGraphic_Device }
{
}

CBird_Bullet::CBird_Bullet(const CBird_Bullet& Prototype)
	: CBullet{ Prototype }
{
}


HRESULT CBird_Bullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBird_Bullet::Initialize(void* pArg)
{
	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);
	if (FAILED(Ready_Components()))
		return E_FAIL;
	//m_pAnimatorCom->Change_Texture(TEXT("Card_Bullet"));
	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	//m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), D3DXToRadian(m_ptBullet_DESC->fAngle));
	m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_ptBullet_DESC->m_Pos.x, 1.9f, m_ptBullet_DESC->m_Pos.z));
	m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Scaling(.5f, .5f, .5f);

	m_fTarget_Vector = m_ptBullet_DESC->m_Pos - m_ptBullet_DESC->m_targetPos;

	m_BulletRange = 5.f;

	m_pColliderCubeCom->Scaling(0.3f, 1.f, 0.3f);

	RandCard = rand() % 5;

	return S_OK;
}

_uint CBird_Bullet::Priority_Update(_float fTimeDelta)
{

	if (m_bdead == true)
	{
		return OBJ_DEAD;
	}
	_float fResult, fr;
	fResult = powf(m_OriginPos.x - m_pTransformCom->Get_State(CTransform::STATE_POSITION).x, 2) +
		powf(m_OriginPos.z - m_pTransformCom->Get_State(CTransform::STATE_POSITION).z, 2);

	fr = (_float)pow(m_BulletRange + 1.f, 2);

	if (fResult >= fr)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CBird_Bullet::Update(_float fTimeDelta)
{
	m_Sprite_Time = fTimeDelta;

	m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 MonsterMatrix;
	MonsterMatrix = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&m_fTarget_Vector, &m_fTarget_Vector);
	MonsterMatrix = MonsterMatrix - m_fTarget_Vector * fTimeDelta * 3;
	MonsterMatrix.y = 1.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, MonsterMatrix);
}

void CBird_Bullet::Late_Update(_float fTimeDelta)
{
	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBird_Bullet::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	if (FAILED(m_pColliderCubeCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBird_Bullet::Ready_Components()
{

	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bird_Bullet"),
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBird_Bullet::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;

}

HRESULT CBird_Bullet::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CBird_Bullet::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (strLayerTag == TEXT("Layer_Player"))
	{
		m_bdead = true;
	}

	return S_OK;
}

CBird_Bullet* CBird_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CBird_Bullet* pInstance = new CBird_Bullet(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBird_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBird_Bullet::Clone(void* pArg)
{
	CBird_Bullet* pInstance = new CBird_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBird_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBird_Bullet::Free()
{
	__super::Free();
}
