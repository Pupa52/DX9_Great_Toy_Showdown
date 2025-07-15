#include "stdafx.h"
#include "Electric_Bolt.h"
#include "GameInstance.h"

#include "Monster.h"
#include "Bot.h"
#include "Damage.h"

CElectric_Bolt::CElectric_Bolt(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CSkill(pGraphic_Device)
{
}

CElectric_Bolt::CElectric_Bolt(const CElectric_Bolt& Prototype)
	: CSkill(Prototype)
{
}

HRESULT CElectric_Bolt::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElectric_Bolt::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	SKILL_DESC desc = *static_cast<SKILL_DESC*>(pArg);
	m_eLevelID = desc.eLevelID;

	switch (desc.iRank)
	{
	case 0:
		m_iAttack = 5;
		break;

	case 1:
		m_iAttack = 10;
		break;

	case 2:
		m_iAttack = 15;
		break;
		
	default:
		return E_FAIL;
	}
	m_pTransformCom->Scaling(1.f, 4.f, 1.f);
	m_pColliderCom->Scaling(4.f, 4.f, 4.f);
	desc.vPos.y += 1.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, desc.vPos);


	m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_SKILL));
	m_pGameInstance->SoundPlay(L"WeaponSkill_BubbleShield_Spawn.wav", _uint(CHANNELID::SOUND_SKILL), .8f);

	_float3 PlayerPos = desc.vPos;
	_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
		PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
	{
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_SKILL));
	}
	return S_OK;
}

_uint CElectric_Bolt::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
	{
		
		
		return OBJ_DEAD;
	}

	if (m_fTextureIndes > 2.f)
	{
		m_fTextureIndes = 0.f;
		++m_iFrameCount;
	}
	if (m_iFrameCount > 1)
		m_bDead = true;

	return OBJ_NOEVENT;
}

void CElectric_Bolt::Update(_float fTimeDelta)
{
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CElectric_Bolt::Late_Update(_float fTimeDelta)
{
	m_fTextureIndes += fTimeDelta * 10.f;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CElectric_Bolt::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fTextureIndes)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CElectric_Bolt::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	/*if (TEXT("Layer_Monster") == strLayerTag && false == m_bHit)
	{
		CMonster* pMonster = static_cast<CMonster*>(pOther);
		pMonster->Set_Monster_Statue(CMonster::MONSTER_STATE_STUN);
		
		pMonster->Set_Hp(pMonster->Get_Hp() - m_iAttack);

		CDamage::DAMAGE_DESC desc = {};
		desc.iDamage = m_iAttack;
		desc.vPos = static_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
			TEXT("Prototype_GameObject_Damage"), &desc);

		m_bHit = true;
	}

	if (TEXT("Layer_Bot") == strLayerTag && false == m_bHit)
	{
		CBot* pBot = static_cast<CBot*>(pOther);
		pBot->Set_Bot_Statue(CBot::BOT_STATE_STUN);

		pBot->Set_Hp(pBot->Get_Hp() - m_iAttack);

		CDamage::DAMAGE_DESC desc = {};
		desc.iDamage = m_iAttack;
		desc.vPos = static_cast<CTransform*>(pBot->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
			TEXT("Prototype_GameObject_Damage"), &desc);

		m_bHit = true;
	}*/

	return S_OK;
}

HRESULT CElectric_Bolt::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Electric_Bolt"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CElectric_Bolt::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	return S_OK;
}

HRESULT CElectric_Bolt::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CElectric_Bolt* CElectric_Bolt::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CElectric_Bolt* pInstance = new CElectric_Bolt(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CElectric_Bolt"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElectric_Bolt::Clone(void* pArg)
{
	CElectric_Bolt* pInstance = new CElectric_Bolt(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CElectric_Bolt"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElectric_Bolt::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
}
