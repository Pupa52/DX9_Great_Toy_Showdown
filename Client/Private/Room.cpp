#include "stdafx.h"
#include "..\Public\Room.h"

#include "GameInstance.h"

CRoom::CRoom(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CRoom::CRoom(const CRoom& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CRoom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRoom::Initialize(void* pArg)
{
	ROOM_DESC* desc = static_cast<ROOM_DESC*>(pArg);

	m_bColl = desc->bColl;
	m_eRoom = (ROOM)desc->iRoom;
	m_eLevelID = desc->eLevelID;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if(LEVEL_GAMEPLAY == m_eLevelID)
		desc->vPos.y += 0.5f * desc->fSize + 0.02f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, desc->vPos);
	m_pTransformCom->Scaling(desc->fSize, desc->fSize, desc->fSize);

	m_fSize = desc->fSize;
	m_fCollSize = desc->fCollSize;
	m_iTextureIndex = desc->iIndex;
	m_bLayDown = desc->bLayDown;

	if (true == m_bColl)
	{
		m_pColliderCom->Set_State(CCollider::STATE_POSITION, desc->vPos);
		m_pColliderCom->Scaling(desc->fCollSize, desc->fSize, desc->fCollSize);
	}
	if (true == m_bLayDown)
	{
		m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));
		desc->vPos.y -= 0.5f * desc->fSize;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, desc->vPos);

	}

	return S_OK;
}

_uint CRoom::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CRoom::Update(_float fTimeDelta)
{

}

void CRoom::Late_Update(_float fTimeDelta)
{
	if (true == m_bColl)
		m_pColliderCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CRoom::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTexture->Bind_Texture(m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	if (true == m_bColl)
	{
		if (FAILED(m_pColliderCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRoom::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CRoom::Ready_Components()
{
	if (true == m_bColl)
	{
		/* For.Com_Collider_Cube */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
			TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), nullptr)))
			return E_FAIL;
	}

	_wstring strPrototypeTag = {};
	if (ROOM_MAGIC == m_eRoom)
		strPrototypeTag = TEXT("Prototype_Component_Texture_Room_Magic");
	else if (ROOM_MUSEUM == m_eRoom)
		strPrototypeTag = TEXT("Prototype_Component_Texture_Room_Museum");
	else if (ROOM_PHONE == m_eRoom)
		strPrototypeTag = TEXT("Prototype_Component_Texture_Room_Phone");
	else if (ROOM_PIERROT == m_eRoom)
		strPrototypeTag = TEXT("Prototype_Component_Texture_Room_Pierrot");
	else if (ROOM_BACK == m_eRoom)
		strPrototypeTag = TEXT("Prototype_Component_Texture_Room_Back");
	else if (ROOM_DOOR == m_eRoom)
		strPrototypeTag = TEXT("Prototype_Component_Texture_Room_Door");
	else if (ROOM_GROUND == m_eRoom)
		strPrototypeTag = TEXT("Prototype_Component_Texture_Room_Ground");
	else if (ROOM_ROBBY == m_eRoom)
		strPrototypeTag = TEXT("Prototype_Component_Texture_Room_Robby");
	else if (ROOM_TOY == m_eRoom)
		strPrototypeTag = TEXT("Prototype_Component_Texture_Room_Toy");
	else
		return E_FAIL;

	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(m_eLevelID, strPrototypeTag,
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTexture), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRoom::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);

	return S_OK;
}

HRESULT CRoom::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CRoom* CRoom::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRoom* pInstance = new CRoom(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRoom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CRoom::Clone(void* pArg)
{
	CRoom* pInstance = new CRoom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRoom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoom::Free()
{
	__super::Free();

	Safe_Release(m_pTexture);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
}
