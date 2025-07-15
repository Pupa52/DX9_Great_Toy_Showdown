#include "stdafx.h"
#include "..\Public\Effect_Shadow.h"

#include "GameInstance.h"
#include "Monster.h"
#include "Player.h"

#include "LandObject.h"
#include "Layer.h"

CEffect_Shadow::CEffect_Shadow(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Shadow::CEffect_Shadow(const CEffect_Shadow& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Shadow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Shadow::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x+3, .1f, m_Edesc->m_Pos.z-3.f));

	m_fScale = { .4f, .4f, .4f };
	
	m_pAnimatorCom->Change_Texture(TEXT("Effect_Shadow"));

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
	
	return S_OK;
}

_uint CEffect_Shadow::Priority_Update(_float fTimeDelta)
{
	if (m_bdead)
		return OBJ_DEAD;


	if (!m_bPlayer)
	{
		if (static_cast<CMonster*>(m_pMonster)->Get_Dead())
		{
			m_bdead = true;
			m_pMonster = nullptr;
		}
		if (m_pMonster != nullptr)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(static_cast<CMonster*>(m_pMonster)->Get_Pos().x, 1.1f, static_cast<CMonster*>(m_pMonster)->Get_Pos().z));
		}
	}
	else if (m_bPlayer)
	{
		if (static_cast<CPlayer*>(m_pMonster)->Get_Dead())
		{
			m_bdead = true;
			m_pMonster = nullptr;
		}
		if (m_pMonster != nullptr)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(static_cast<CPlayer*>(m_pMonster)->Get_Pos().x, 1.1f, static_cast<CPlayer*>(m_pMonster)->Get_Pos().z));
		}
	}

	return OBJ_NOEVENT;
}

void CEffect_Shadow::Update(_float fTimeDelta)
{
	
	m_EffectCount += fTimeDelta;

	m_fFrame += 90.0f * fTimeDelta;

	if (m_fFrame >= 90.0f)
	{
		m_FrameCount++;
		m_fFrame = 0.f;
	}

	//Find_Obj();
	//CLandObject::SetUp_OnTerrain(m_pTransformCom, 1.5f);

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Shadow::Late_Update(_float fTimeDelta)
{
//	m_bdead = true;

	m_pAnimatorCom->Move_Frame(fTimeDelta);

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);


}

HRESULT CEffect_Shadow::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pAnimatorCom->Bind_Texture()))
		return E_FAIL;
	
	//if (FAILED(m_pTextureCom->Bind_Texture(0)))
	//	return E_FAIL;

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

HRESULT CEffect_Shadow::Find_Obj()
{
	/*_float3 _TempPos;
	
	CLayer* MonsterLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Monster")));
	CLayer* PlayerLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Player")));
	CLayer* BotLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Bot")));
	
	if (nullptr == MonsterLayer || nullptr == PlayerLayer || nullptr == BotLayer)
		return E_FAIL;
	
	list<CGameObject*> Monster_Obj = MonsterLayer->Get_Objects();
	list<CGameObject*> Player_Obj = PlayerLayer->Get_Objects();
	list<CGameObject*> Bot_Obj = BotLayer->Get_Objects();

	for (auto& pDst : Monster_Obj)
	{
		if (!static_cast<CMonster*>(pDst)->Get_Shadow())
		{
			static_cast<CMonster*>(pDst)->Set_Shadow();
			m_pObj.push_back(pDst);
		}
	}
	for (auto& pObj : m_pObj)
	{
		if (pObj != nullptr && static_cast<CMonster*>(pObj)->Get_Shadow())
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_Edesc->m_Pos);
		}
		else if (static_cast<CMonster*>(pObj)->Get_Dead())
		{
			m_bdead = true;
			pObj == nullptr;
		}
	}*/


	return S_OK;
}

HRESULT CEffect_Shadow::Ready_Components()
{

	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pAnimatorCom->Add_Texture(TEXT("Effect_Shadow"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Shadow"))));

	///* For.Com_Textrue */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Shadow"),
	//	TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
	//	return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 0.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Shadow::SetUp_RenderState()
{
	//_float3		vSourColor, vDestColor;
	/*_float3		vResultColor =
		vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a)
	*/

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CEffect_Shadow::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Shadow* CEffect_Shadow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Shadow* pInstance = new CEffect_Shadow(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Shadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Shadow::Clone(void* pArg)
{
	CEffect_Shadow* pInstance = new CEffect_Shadow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Shadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Shadow::Free()
{
	__super::Free();

	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pTextureCom);
	//Safe_Release(m_pVIBufferCom);
}
