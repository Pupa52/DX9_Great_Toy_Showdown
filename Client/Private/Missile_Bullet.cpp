#include "stdafx.h"
#include "Missile_Bullet.h"
#include "Layer.h"
#include "Monster.h"
#include "Bot.h"

#include "Effect.h"
#include "Damage.h"

CMissile_Bullet::CMissile_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet(pGraphic_Device)
{
}

CMissile_Bullet::CMissile_Bullet(const CMissile_Bullet& Prototype)
	:CBullet(Prototype)
{
}

HRESULT CMissile_Bullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMissile_Bullet::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ptBullet_DESC->m_Pos);
	m_iDamage = 30;

	m_pColliderCubeCom->Scaling(1.f, 1.f, 1.f);

	return Find_Target();
}

_uint CMissile_Bullet::Priority_Update(_float fTimeDelta)
{
	if (true == m_bdead)
	{

		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_Laser));
		m_pGameInstance->SoundPlay(L"Drop_SmallExplosion.wav", _uint(CHANNELID::SOUND_Laser), .5f);

		CEffect::EFFECT_DESC desc = {};
		desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Missile_Boom"),
			&desc);

		return OBJ_DEAD;
	}

	if (true == m_bActive)
	{
		m_pColliderCubeCom->Scaling(2.f, 2.f, 2.f);
		m_bdead = true;
	}

	return OBJ_NOEVENT;
}

void CMissile_Bullet::Update(_float fTimeDelta)
{
	if (false == m_bActive)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_fTarget_Vector * fTimeDelta * m_fSpeed);
		m_BulletRange += m_fSpeed * fTimeDelta;

		m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
}

void CMissile_Bullet::Late_Update(_float fTimeDelta)
{
	if (m_BulletRange > 10.f)
	{
		m_bActive = true;
	}

	m_fSpeed += 0.1f;
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMissile_Bullet::Render()
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

HRESULT CMissile_Bullet::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	m_bActive = true;
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
	if (TEXT("Layer_Bot") == strLayerTag)
	{
	}
	return S_OK;
}

HRESULT CMissile_Bullet::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Missile_Bullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMissile_Bullet::SetUp_RenderState()
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

HRESULT CMissile_Bullet::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;

}

HRESULT CMissile_Bullet::Find_Target()
{
	CLayer* pMonsterLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
	CLayer* pBotLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Bot"));
	if (nullptr == pBotLayer && nullptr == pMonsterLayer)
	{
		m_bdead = true;
		return S_OK;
	}

	auto& MonsterList = pMonsterLayer->Get_Objects();
	auto& BotList = pBotLayer->Get_Objects();

	CMonster* pMonster = nullptr;
	CBot* pBot = nullptr;

	if(nullptr != pMonsterLayer && false == MonsterList.empty())
	{
		pMonster = static_cast<CMonster*>(*MonsterList.begin());

		for (auto& elem : MonsterList)
		{
			_float3 vDir = static_cast<CMonster*>(elem)->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float3 vTargetDir = pMonster->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if (D3DXVec3Length(&vDir) < D3DXVec3Length(&vTargetDir))
				pMonster = static_cast<CMonster*>(elem);
		}
	}


	if(nullptr != pBotLayer && false == BotList.empty())
	{
		pBot = static_cast<CBot*>(*BotList.begin());

		for (auto& elem : BotList)
		{
			_float3 vDir = static_cast<CBot*>(elem)->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float3 vTargetDir = pBot->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if (D3DXVec3Length(&vDir) < D3DXVec3Length(&vTargetDir))
				pBot = static_cast<CBot*>(elem);
		}
	}

	if (nullptr != pBotLayer && false == BotList.empty() && nullptr != pMonsterLayer && false == MonsterList.empty())
	{
		_float3 vMonsterDir = pMonster->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vBotDir = pBot->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (D3DXVec3Length(&vMonsterDir) > D3DXVec3Length(&vBotDir))
		{
			m_fTarget_Vector = *D3DXVec3Normalize(&vBotDir, &vBotDir);
			m_ptBullet_DESC->m_targetPos = pBot->Get_Pos();
		}
		else
		{
			m_fTarget_Vector = *D3DXVec3Normalize(&vMonsterDir, &vMonsterDir);
			m_ptBullet_DESC->m_targetPos = pMonster->Get_Pos();
		}
	}
	else if (nullptr == pBotLayer || true == BotList.empty())
	{
		_float3 vMonsterDir = pMonster->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_fTarget_Vector = *D3DXVec3Normalize(&vMonsterDir, &vMonsterDir);
		m_ptBullet_DESC->m_targetPos = pMonster->Get_Pos();
	}
	else if (nullptr == pMonsterLayer || true == MonsterList.empty())
	{
		_float3 vBotDir = pBot->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_fTarget_Vector = *D3DXVec3Normalize(&vBotDir, &vBotDir);
		m_ptBullet_DESC->m_targetPos = pBot->Get_Pos();

	}

	_float fDot = D3DXVec3Dot(&m_fTarget_Vector, &_float3(1.f, 0.f, 0.f));
	m_ptBullet_DESC->fAngle = acos(fDot);

	if (m_ptBullet_DESC->m_targetPos.z < m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		m_ptBullet_DESC->fAngle *= -1.f;

	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), m_ptBullet_DESC->fAngle);
	m_fTarget_Vector.y = 0.f;


	return S_OK;
}


CMissile_Bullet* CMissile_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMissile_Bullet* pInstance = new CMissile_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create_Failed : CMissile_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CMissile_Bullet::Clone(void* pArg)
{
	CMissile_Bullet* pInstance = new CMissile_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone_Failed : CMissile_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMissile_Bullet::Free()
{
	__super::Free();
}
