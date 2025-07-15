#include "stdafx.h"
#include "Meteor_Bullet.h"
#include "Effect_Meteor_Ground.h"
#include "Monster.h"
#include "Damage.h"

CMeteor_Bullet::CMeteor_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet(pGraphic_Device)
{
}

CMeteor_Bullet::CMeteor_Bullet(const CMeteor_Bullet& Prototype)
	: CBullet(Prototype)
{
}

HRESULT CMeteor_Bullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMeteor_Bullet::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_SKILL));
	m_pGameInstance->SoundPlay(L"SFX_Skill_Falling.wav", _uint(CHANNELID::SOUND_PLAYER_SKILL), .5f);

	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);

	m_iDamage = 100;
	m_fSpeed = 5.f;
	// 목표 위치

	// 시작 위치
	m_ptBullet_DESC->m_Pos = m_ptBullet_DESC->m_targetPos;
	m_ptBullet_DESC->m_Pos.x = m_ptBullet_DESC->m_targetPos.x + 2;
	m_ptBullet_DESC->m_Pos.y = m_ptBullet_DESC->m_targetPos.y + 5;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ptBullet_DESC->m_Pos);

	// 떨어지는 각도 계산(이미지가 아래 방향이라 + 90 해줌.
	m_fTarget_Vector = m_ptBullet_DESC->m_targetPos - m_ptBullet_DESC->m_Pos;
	D3DXVec3Normalize(&m_fTarget_Vector, &m_fTarget_Vector);

	_float fDot = D3DXVec3Dot(&m_fTarget_Vector, &_float3(1.f, 0.f, 0.f));
	m_ptBullet_DESC->fAngle = acos(fDot) * -1.f;
	m_ptBullet_DESC->fAngle += D3DXToRadian(90.f);

	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), m_ptBullet_DESC->fAngle);

	m_pTransformCom->Scaling(1.5f, 1.5f, 1.5f);
	m_pColliderCubeCom->Scaling(3.f, 3.f, 3.f);

	CEffect_Meteor_Ground::METEORGROUND_DESC desc = {};
	desc.m_Pos = m_ptBullet_DESC->m_targetPos;
	desc.pDead = &m_bdead;
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Meteor_Ground"),
		&desc);
	
	
	return S_OK;
}

_uint CMeteor_Bullet::Priority_Update(_float fTimeDelta)
{
	if (true == m_bdead)
	{
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_Laser));
		m_pGameInstance->SoundPlay(L"Drop_SmallExplosion.wav", _uint(CHANNELID::SOUND_Laser), 1.f);

		CEffect::EFFECT_DESC desc= {};
		desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Meteor_Explosion"),
			&desc);
		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CMeteor_Bullet::Update(_float fTimeDelta)
{
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPos += m_fTarget_Vector * fTimeDelta * m_fSpeed;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CMeteor_Bullet::Late_Update(_float fTimeDelta)
{
	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < 1.f)
	{
		m_bdead = true;
	}
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

}

HRESULT CMeteor_Bullet::Render()
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

HRESULT CMeteor_Bullet::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
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

HRESULT CMeteor_Bullet::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Meteor_Bullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeteor_Bullet::SetUp_RenderState()
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

HRESULT CMeteor_Bullet::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CMeteor_Bullet* CMeteor_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMeteor_Bullet* pInstance = new CMeteor_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create_Failed : CMonster_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMeteor_Bullet::Clone(void* pArg)
{
	CMeteor_Bullet* pInstance = new CMeteor_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone_Failed : CMonster_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeteor_Bullet::Free()
{
	__super::Free();
}
