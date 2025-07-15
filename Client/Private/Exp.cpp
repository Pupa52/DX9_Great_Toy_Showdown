#include "stdafx.h"
#include "Exp.h"
#include "GameInstance.h"

#include "Player.h"
CExp::CExp(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CItem(pGraphic_Device)
{
}

CExp::CExp(const CExp& Prototype)
	:CItem(Prototype)
{
}

HRESULT CExp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CExp::Initialize(void* pArg)
{
	m_iRandCount = rand() % 3;
	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	m_pDesc = static_cast<ITEM_DESC*>(pArg);
	m_eItemType = m_pDesc->eItemType;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_vPos = m_pDesc->vPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pDesc->vPos);
	m_pTransformCom->Scaling(.2f, .2f, .2f);
	m_pColliderCom->Scaling(.2f, .2f, .2f);

	return S_OK;
}

_uint CExp::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
	{
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPITEM));
		m_pGameInstance->SoundPlay(L"Player_GetExp.wav", _uint(CHANNELID::SOUND_EXPITEM), 1.f);
		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CExp::Update(_float fTimeDelta)
{
	m_fDeltaCount += fTimeDelta;
	m_fAirTime += fTimeDelta;
	CPlayer::CActor::ACTOR_DESC tDesc = {};

	_float3 vPlayerPos;
	_float3 vITemPos;
	tDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vITemPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (!m_bCanGetExp)
	{
		if (m_fAirTime >= 1)
		{
			m_fAirTime = 1;
		}
		_float result = 2.f * m_fAirTime - 0.5f * (m_fGravity * (m_fAirTime * m_fAirTime));
		vITemPos.y += result;
	}
	if (vITemPos.y <= 1.3f)
	{
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Impact"), &tDesc);

		m_iBounceCount--;
		m_fAirTime = 0.f;
		m_fGravity *= 2;
		vITemPos.y = 2.f;
	}
	if (m_iBounceCount <= 0)
	{
		vITemPos.y = 1.3f;
		m_bCanGetExp = true;
	}
	D3DXVECTOR3 dir = vITemPos - vPlayerPos;
	D3DXVec3Normalize(&dir, &dir);
	vITemPos = vITemPos - dir * fTimeDelta * m_fSpeed;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vITemPos);

	if (m_fDeltaCount >= 0.001f)
	{
//		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Exp_Trail"), &tDesc);
		m_fDeltaCount = 0;
		m_fSpeed+= 0.05f;
	}
	__super::Update(fTimeDelta);

}

void CExp::Late_Update(_float fTimeDelta)
{
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CExp::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(m_iRandCount)))
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

HRESULT CExp::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (strLayerTag == TEXT("Layer_Player"))
	{


		CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
		pPlayer->Set_Exp(1);

		m_bDead = true;
	}


	return S_OK;
}

HRESULT CExp::Ready_Component()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Exp"),
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

CExp* CExp::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CExp* pInstance = new CExp(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CExp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CExp::Clone(void* pArg)
{
	CExp* pInstance = new CExp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CExp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExp::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
