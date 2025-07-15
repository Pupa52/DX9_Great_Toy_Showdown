#include "stdafx.h"
#include "GumBoom.h"
#include "GameInstance.h"

#include "Monster.h"
#include "Bot.h"

#include "Effect.h"
#include "Damage.h"

CGumBoom::CGumBoom(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CSkill(pGraphic_Device)
{
}

CGumBoom::CGumBoom(const CGumBoom& Prototype)
	: CSkill(Prototype)
{
}

HRESULT CGumBoom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGumBoom::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	SKILL_GUM_DESC* pDesc = static_cast<SKILL_GUM_DESC*>(pArg);

	m_iRank = pDesc->iRank;
	m_vTargetPos = pDesc->vTargetPos;
	m_vStartPos = pDesc->vPos;
	m_eLevelID = pDesc->eLevelID;
	m_vTargetDir = m_vTargetPos - m_vStartPos;
	_float vDist = D3DXVec3Length(&m_vTargetDir);
	m_fSpeed = vDist;
	D3DXVec3Normalize(&m_vTargetDir, &m_vTargetDir);


	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(m_eLevelID, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (nullptr == m_pPlayerTransform)
		return E_FAIL;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vStartPos);
	m_pTransformCom->Scaling(.5f, .5f, .5f);

	switch (m_iRank)
	{
	case 0:
		m_fEffectArea = 3.f;
		m_fActiveTime = 3.f;
		break;

	case 1:
		m_fEffectArea = 4.f;
		m_fActiveTime = 5.f;
		break;

	case 2:
		m_fEffectArea = 5.f;
		m_fActiveTime = 7.f;
		break;

	default:
		return E_FAIL;
	}


	m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_SKILL));
	m_pGameInstance->SoundPlay(L"sfx_WeaponSkillShot_Candy.wav", _uint(CHANNELID::SOUND_SKILL), .8f);

	_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
		PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
	{
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_SKILL));
	}
	
	m_fForce = 4.f;
	m_fFistForce = m_fForce;

	m_iAttack = 10;
	m_eCharacter = PLAYER_CHARACTER::PLAYER_CAT;

	return S_OK;
}

_uint CGumBoom::Priority_Update(_float fTimeDelta)
{
	if (m_fActiveTime < 0.f)
		return OBJ_DEAD;

	if (true == m_bActive)
	{
		m_fActiveTime -= fTimeDelta;
		m_pColliderCom->Scaling(m_fEffectArea, m_fEffectArea, m_fEffectArea);
		m_pTransformCom->Scaling(m_fEffectArea, m_fEffectArea, m_fEffectArea);
	}

	return OBJ_NOEVENT;
}

void CGumBoom::Update(_float fTimeDelta)
{
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if(false == m_bActive)
	{
		vPos += m_vTargetDir * m_fSpeed * fTimeDelta;
		vPos.y += m_fForce * fTimeDelta;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_fForce -= m_fFistForce * fTimeDelta * 2;
	}

	if (true == m_bActive && m_fSplashTime > .2f)
	{
		_int iOverNum = 2 + m_iRank;
		// 여기서 떨구는 거.
		_int iMoveX = GetRandomNum(0, iOverNum);
		_int iMoveZ = GetRandomNum(0, iOverNum);

		if (GetRandomNum(0, 1))
			iMoveX *= -1;
		if (GetRandomNum(0, 1))
			iMoveZ *= -1;

		CEffect::EFFECT_DESC desc = {};
		desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		desc.m_Pos.x += (_float)iMoveX * .5f;
		desc.m_Pos.z += (_float)iMoveZ * .5f;
		desc.m_Pos.y = 1.5f;
		if(GetRandomNum(0, 1))
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Effect_Gum_Small_Splash"), &desc)))
				return;
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Effect_Gum_Big_Splash"), &desc)))
				return;
		}
		m_fSplashTime = 0.f;
	}
	m_pColliderCom->Update_Collider(vPos);
}

void CGumBoom::Late_Update(_float fTimeDelta)
{
	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < 1.1f && false == m_bActive)
	{
		m_bActive = true;
		m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));

		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.y = 1.1f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		CEffect::EFFECT_DESC desc = {};
		desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		desc.m_Pos.y = 2.f;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
			TEXT("Prototype_GameObject_Effect_Gum_Skul"), &desc)))
			return;
	}
	m_fSplashTime += fTimeDelta;
	m_fDamageDelay += fTimeDelta;

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CGumBoom::Render()
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

HRESULT CGumBoom::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (LEVEL_GAMEPLAY == m_eLevelID)
	{
		if (TEXT("Layer_Monster") == strLayerTag && true == m_bActive)
		{
			CMonster* pMonster = static_cast<CMonster*>(pOther);
			pMonster->Set_Monster_Statue(CMonster::MONSTER_STATE_SLOW); // 슬로우, 도트딜.

		}
		if (TEXT("Layer_Bot") == strLayerTag && true == m_bActive)
		{
			CBot* pBot = static_cast<CBot*>(pOther);
			pBot->Set_Bot_Statue(CBot::BOT_STATE_SLOW); // 슬로우, 도트딜.

			pBot->Set_Hp(pBot->Get_Hp() - m_iAttack);

			CDamage::DAMAGE_DESC desc = {};
			desc.iDamage = m_iAttack;
			desc.vPos = static_cast<CTransform*>(pBot->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
				TEXT("Prototype_GameObject_Damage"), &desc);
		}
	}
	return S_OK;
}

HRESULT CGumBoom::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Skill_Gum"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGumBoom::SetUp_RenderState()
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

HRESULT CGumBoom::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CGumBoom* CGumBoom::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGumBoom* pInstance = new CGumBoom(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CGumBoom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGumBoom::Clone(void* pArg)
{
	CGumBoom* pInstance = new CGumBoom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CGumBoom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGumBoom::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);

}
