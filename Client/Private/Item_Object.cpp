#include "stdafx.h"
#include "Item_Object.h"
#include "GameInstance.h"

#include "Player.h"

#include "Item_Meteor.h"
#include "Item_Mine.h"
#include "Item_Missile.h"
#include "Item_Teleport.h"
#include "Item_Passive.h"


CItem_Object::CItem_Object(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CItem(pGraphic_Device)
{	
}

CItem_Object::CItem_Object(const CItem_Object& Prototype)
	:CItem(Prototype)
{
}

HRESULT CItem_Object::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Object::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);
	m_eItemType = pDesc->eItemType;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

	return S_OK;
}

_uint CItem_Object::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
	{
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_GETITEM));
		m_pGameInstance->SoundPlay(L"MapObj_ItemBox_Hit.wav", _uint(CHANNELID::SOUND_GETITEM), .8f);

		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CItem_Object::Update(_float fTimeDelta)
{
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CItem_Object::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CItem_Object::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(__super::SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(__super::Reset_RenderState()))
		return E_FAIL;

	m_pColliderCom->Render();

	return S_OK;
}

HRESULT CItem_Object::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (TEXT("Layer_Player") == strLayerTag && m_pGameInstance->Key_Down('F'))
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
		switch (m_eItemType)
		{
		case ITEM_SWORD:
			pPlayer->Add_Item(TEXT("Item_Swotd"), CItem_Passive::Create(pPlayer, (_uint)ITEM_SWORD));
			break;

		case ITEM_SKATE:
			pPlayer->Add_Item(TEXT("Item_Skate"), CItem_Passive::Create(pPlayer, (_uint)ITEM_SKATE));
			break;

		case ITEM_MAGAZINE:
			pPlayer->Add_Item(TEXT("Item_Magazine"), CItem_Passive::Create(pPlayer, (_uint)ITEM_MAGAZINE));
			break;

		case ITEM_MISSILE:
			pPlayer->Add_Item(TEXT("Item_Missile"), CItem_Missile::Create(pPlayer));
			break;

		case  ITEM_METEOR:
			pPlayer->Add_Item(TEXT("Item_Meteor"), CItem_Meteor::Create(pPlayer));
			break;

		case ITEM_MINE:
			pPlayer->Add_Item(TEXT("Item_Mine"), CItem_Mine::Create(pPlayer));
			break;

		case ITEM_TELEPORT:
			pPlayer->Add_Item(TEXT("Item_Teleport"), CItem_Teleport::Create(pPlayer));
			break;

		default:
			return E_FAIL;
		}

		m_bDead = true;
	}

	return S_OK;
}

HRESULT CItem_Object::Ready_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), nullptr)))
		return E_FAIL;
	
	wstring strItemTag = TEXT("");

	switch (m_eItemType)
	{
	case ITEM_SWORD:
		strItemTag = TEXT("Prototype_Component_Texture_Item_Sword");
		break;

	case ITEM_SKATE:
		strItemTag = TEXT("Prototype_Component_Texture_Item_Skate");
		break;

	case ITEM_MAGAZINE:
		strItemTag = TEXT("Prototype_Component_Texture_Item_Magazine");
		break;

	case ITEM_MISSILE:
		strItemTag = TEXT("Prototype_Component_Texture_Item_Missile");
		break;

	case  ITEM_METEOR:
		strItemTag = TEXT("Prototype_Component_Texture_Item_Meteor");
		break;

	case ITEM_MINE:
		strItemTag = TEXT("Prototype_Component_Texture_Item_Mine");
		break;

	case ITEM_TELEPORT:
		strItemTag = TEXT("Prototype_Component_Texture_Item_Teleport");
		break;


	default:
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strItemTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}


CItem_Object* CItem_Object::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItem_Object* pInstance = new CItem_Object(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CItem_Object"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Object::Clone(void* pArg)
{
	CItem_Object* pInstance = new CItem_Object(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CItem_Object"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Object::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
