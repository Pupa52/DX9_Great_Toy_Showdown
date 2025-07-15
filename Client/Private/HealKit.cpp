#include "stdafx.h"
#include "Healkit.h"
#include "GameInstance.h"

#include "Player.h"
CHealkit::CHealkit(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CItem(pGraphic_Device)
{
}

CHealkit::CHealkit(const CHealkit& Prototype)
	:CItem(Prototype)
{
}

HRESULT CHealkit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHealkit::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pDesc = static_cast<ITEM_DESC*>(pArg);
	m_eItemType = m_pDesc->eItemType;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_vPos = m_pDesc->vPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pDesc->vPos);
	m_pTransformCom->Scaling(1.f, 1.f, 1.f);
	m_pColliderCom->Scaling(.5f, .5f, .5f);

	return S_OK;
}

_uint CHealkit::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
	{
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPITEM));
		m_pGameInstance->SoundPlay(L"MapObj_ItemBox_Hit.wav", _uint(CHANNELID::SOUND_EXPITEM), 1.f);
		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CHealkit::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CHealkit::Late_Update(_float fTimeDelta)
{
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CHealkit::Render()
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

	if (FAILED(m_pColliderCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHealkit::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (strLayerTag == TEXT("Layer_Player"))
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
		if (pPlayer->Get_Player_Desc().iMaxHp > pPlayer->Get_ActorDesc().iHp + 50)
		{
			pPlayer->Set_Hp(50);
		}
		else
		{
			_int Heal_iHp;

			Heal_iHp = pPlayer->Get_Player_Desc().iMaxHp - pPlayer->Get_ActorDesc().iHp;
			pPlayer->Set_Hp(Heal_iHp);
		}
		m_bDead = true;
	}


	return S_OK;
}

HRESULT CHealkit::Ready_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
	//------
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_Healkit"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), nullptr)))
		return E_FAIL;


	return S_OK;
}

CHealkit* CHealkit::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHealkit* pInstance = new CHealkit(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CHealkit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHealkit::Clone(void* pArg)
{
	CHealkit* pInstance = new CHealkit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CHealkit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHealkit::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

