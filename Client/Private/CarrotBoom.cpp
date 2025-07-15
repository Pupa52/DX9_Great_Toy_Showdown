#include "stdafx.h"
#include "CarrotBoom.h"
#include "GameInstance.h"

#include "Bullet.h"
#include "Monster.h"
#include "Effect_LandMine_Explosion.h"
#include "Bot.h"
#include "Damage.h"

CCarrotBoom::CCarrotBoom(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CSkill(pGraphic_Device)
{
}

CCarrotBoom::CCarrotBoom(const CCarrotBoom& Prototype)
	: CSkill(Prototype)
{
}

HRESULT CCarrotBoom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarrotBoom::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	SKILL_CARROT_DESC* pDesc = static_cast<SKILL_CARROT_DESC*>(pArg);

	m_iRank = pDesc->iRank;
	m_vTargetPos = pDesc->vTargetPos;
	m_eLevelID = pDesc->eLevelID;

	switch (m_iRank)
	{
	case 0:
		m_fEffectArea = 3.f;
		m_iDamage = 50;
		break;

	case 1:
		m_fEffectArea = 4.f;
		m_iDamage = 50;
		break;

	case 2:
		m_fEffectArea = 5.f;
		m_iDamage = 50;
		break;

	default:
		return E_FAIL;
	}

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(m_eLevelID, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_pTransformCom->Scaling(.5f, .5f, .5f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	m_vTargetDir = m_vTargetPos - pDesc->vPos;
	_float vDist = D3DXVec3Length(&m_vTargetDir);
	m_fSpeed = vDist;
	D3DXVec3Normalize(&m_vTargetDir, &m_vTargetDir);


	m_fForce = 4.f;
	m_fFistForce = m_fForce;

	m_iAttack = 50;
	m_eCharacter = PLAYER_CHARACTER::PLAYER_PENGUIN;

	return S_OK;
}

_uint CCarrotBoom::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
	{
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_SKILL));
		m_pGameInstance->SoundPlay(L"WeaponSkill_BombShot_Explosion.wav", _uint(CHANNELID::SOUND_SKILL), .8f);

		_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
			PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
		{
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_SKILL));
		}
		return OBJ_DEAD;
	}

	if (true == m_bActive)
	{
		CEffect_LandMine_Explosion::EXPLOSION_DESC desc;
		desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		desc.fScale = 5.f;
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Mine_Explosive"), &desc);

		m_pColliderCom->Scaling(m_fEffectArea, m_fEffectArea, m_fEffectArea);
		m_bDead = true;
	}

	return OBJ_NOEVENT;
}

void CCarrotBoom::Update(_float fTimeDelta)
{
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if(false == m_bActive)
	{
		vPos += m_vTargetDir * m_fSpeed * fTimeDelta;
		vPos.y += m_fForce * fTimeDelta;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_fForce -= m_fFistForce * fTimeDelta * 2;

		m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), 4 * fTimeDelta);
	}

	// 충돌체 위치 업데이트
	m_pColliderCom->Update_Collider(vPos);
}

void CCarrotBoom::Late_Update(_float fTimeDelta)
{
	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < 1.1f && false == m_bActive)
	{
		m_bActive = true;
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CCarrotBoom::Render()
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

	if (FAILED(m_pColliderCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarrotBoom::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if(LEVEL_GAMEPLAY == m_eLevelID)
	{
		if (true == m_bActive)
		{
			if (TEXT("Layer_Monster") == strLayerTag)
			{
				CMonster* pMonster = static_cast<CMonster*>(pOther);
				pMonster->Set_Monster_Statue(CMonster::MONSTER_STATE_STUN);
				pMonster->Set_Hp(m_iDamage);
				pMonster->Set_Dead();
				CDamage::DAMAGE_DESC desc = {};
				desc.iDamage = m_iDamage;
				desc.vPos = static_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

				m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
					TEXT("Prototype_GameObject_Damage"), &desc);
			}
			if (TEXT("Layer_Bot") == strLayerTag)
			{
				CBot* pBot = static_cast<CBot*>(pOther);
				pBot->Set_Bot_Statue(CBot::BOT_STATE_STUN);

				CDamage::DAMAGE_DESC desc = {};
				desc.iDamage = m_iDamage;
				desc.vPos = static_cast<CTransform*>(pBot->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

				m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
					TEXT("Prototype_GameObject_Damage"), &desc);
			}
		}
	}
	

	return S_OK;
}

HRESULT CCarrotBoom::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), nullptr)))
		return E_FAIL;

	///* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Skill_Boom"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarrotBoom::SetUp_RenderState()
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

HRESULT CCarrotBoom::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

_float CCarrotBoom::Cal_Angle()
{
	m_vTargetDir;
	_float fDot = D3DXVec3Dot(&m_vTargetDir, &_float3(1.f, 0.f, 0.f));

	_float fAngle = acos(fDot);
	if (m_vTargetDir.z < 0)
		fAngle *= -1.f;

	return fAngle;
}

CCarrotBoom* CCarrotBoom::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCarrotBoom* pInstance = new CCarrotBoom(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CCarrotBoom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarrotBoom::Clone(void* pArg)
{
	CCarrotBoom* pInstance = new CCarrotBoom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CCarrotBoom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarrotBoom::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);

}
