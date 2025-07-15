#include "stdafx.h"
#include "..\Public\Telephone_Meteor_Bullet.h"

#include "Player.h"
#include "Effect_LandMine_Explosion.h"

CTelephone_Meteor_Bullet::CTelephone_Meteor_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet{ pGraphic_Device }
{
}

CTelephone_Meteor_Bullet::CTelephone_Meteor_Bullet(const CTelephone_Meteor_Bullet& Prototype)
	: CBullet{ Prototype }
{
}


HRESULT CTelephone_Meteor_Bullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTelephone_Meteor_Bullet::Initialize(void* pArg)
{
	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pAnimatorCom->Change_Texture(TEXT("Drop_Telephone"));

	m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(45.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_ptBullet_DESC->m_Pos.x, m_ptBullet_DESC->m_Pos.y+10, m_ptBullet_DESC->m_Pos.z));

	m_fTarget_Vector = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_ptBullet_DESC->m_targetPos;

	m_pColliderCubeCom->Scaling(1.f, 1.f, 1.f);

	m_pTransformCom->Scaling(1.f, 1.f, 1.f);

	return S_OK;
}

_uint CTelephone_Meteor_Bullet::Priority_Update(_float fTimeDelta)
{

	CEffect_LandMine_Explosion::EXPLOSION_DESC desc;
	desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	desc.fScale = 4.f;

	if (m_bdead)
	{
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Mine_Explosive"), &desc);
		return OBJ_DEAD;
	}

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y <= 1.5f)
	{
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Mine_Explosive"), &desc);
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPLOSIVE));
		m_pGameInstance->SoundPlay(L"Drop_SmallExplosion.wav", _uint(CHANNELID::SOUND_EXPLOSIVE), .5f);
		return OBJ_DEAD;
	}
	return OBJ_NOEVENT;
}

void CTelephone_Meteor_Bullet::Update(_float fTimeDelta)
{
	m_Sprite_Time = fTimeDelta;

	m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	D3DXVec3Normalize(&m_fTarget_Vector, &m_fTarget_Vector);

	m_ptBullet_DESC->m_Pos = m_ptBullet_DESC->m_Pos - m_fTarget_Vector * 10 * fTimeDelta;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ptBullet_DESC->m_Pos);
}

void CTelephone_Meteor_Bullet::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Move_Frame(fTimeDelta);
	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTelephone_Meteor_Bullet::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pAnimatorCom->Bind_Texture()))
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

HRESULT CTelephone_Meteor_Bullet::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{

	if (strLayerTag == TEXT("Layer_Player"))
	{
		m_bdead = true;
	}

	return S_OK;
}

HRESULT CTelephone_Meteor_Bullet::Ready_Components()
{

	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;


	m_pAnimatorCom->Add_Texture(TEXT("Drop_Telephone"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Drop_Telephone"))));

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

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

HRESULT CTelephone_Meteor_Bullet::SetUp_RenderState()
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

HRESULT CTelephone_Meteor_Bullet::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CTelephone_Meteor_Bullet* CTelephone_Meteor_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	
	CTelephone_Meteor_Bullet* pInstance = new CTelephone_Meteor_Bullet(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTelephone_Meteor_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CTelephone_Meteor_Bullet::Clone(void* pArg)
{
	CTelephone_Meteor_Bullet* pInstance = new CTelephone_Meteor_Bullet(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTelephone_Meteor_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTelephone_Meteor_Bullet::Free()
{
	__super::Free();
}
