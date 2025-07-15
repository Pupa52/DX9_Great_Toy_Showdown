#include "stdafx.h"
#include "Mine_Bullet.h"
#include "Damage.h"

#include "Monster.h"
#include "Effect_LandMine_Explosion.h"

CMine_Bullet::CMine_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet(pGraphic_Device)
{
}

CMine_Bullet::CMine_Bullet(const CMine_Bullet& Prototype)
	: CBullet(Prototype)
{
}

HRESULT CMine_Bullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMine_Bullet::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ptBullet_DESC->m_Pos);
	m_iDamage = 50;
	m_fBoomCount = 2.f;
	m_ptBullet_DESC->m_Pos.y -= .49f;
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_PlayerMine_Max")
		, (void*)m_ptBullet_DESC);
	m_ptBullet_DESC->m_Pos.y += .01f;

	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_PlayerMine_Count")
		, (void*)m_ptBullet_DESC);
	m_pColliderCubeCom->Scaling(1.f, 1.f, 1.f);

	return S_OK;
}

_uint CMine_Bullet::Priority_Update(_float fTimeDelta)
{
	if (true == m_bdead)
	{


		CEffect_LandMine_Explosion::EXPLOSION_DESC ptExplosion_Desc;
		ptExplosion_Desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		ptExplosion_Desc.fScale = 3.f;


		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPLOSIVE));
		m_pGameInstance->SoundPlay(L"Drop_SmallExplosion.wav", _uint(CHANNELID::SOUND_EXPLOSIVE), .5f);

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Mine_Explosive")
			, &ptExplosion_Desc);

		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CMine_Bullet::Update(_float fTimeDelta)
{
	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CMine_Bullet::Late_Update(_float fTimeDelta)
{
	if (m_fBoomCount < m_Sprite_Time)
		m_bdead = true;

	m_Sprite_Time += fTimeDelta;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMine_Bullet::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture()))
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

HRESULT CMine_Bullet::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (true == m_bdead)
	{
		if (TEXT("Layer_Monster") == strLayerTag)
		{
			CDamage::DAMAGE_DESC desc = {};
			//desc.iDamage = m_ptBullet_DESC->iAttack;
			desc.iDamage = m_iDamage;

			static_cast<CMonster*>(pOther)->Set_Damage(m_iDamage);
			static_cast<CMonster*>(pOther)->Set_Dead();

			desc.vPos = static_cast<CTransform*>(pOther->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
				TEXT("Prototype_GameObject_Damage"), &desc);
		}
	}

	return S_OK;
}

HRESULT CMine_Bullet::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Lego_LandMine"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMine_Bullet::SetUp_RenderState()
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

HRESULT CMine_Bullet::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CMine_Bullet::Boom()
{
	
}

CMine_Bullet* CMine_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMine_Bullet* pInstance = new CMine_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create_Failed : CMine_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMine_Bullet::Clone(void* pArg)
{
	CMine_Bullet* pInstance = new CMine_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone_Failed : CMine_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMine_Bullet::Free()
{
	__super::Free();
}
