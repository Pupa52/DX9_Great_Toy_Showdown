#include "stdafx.h"
#include "..\Public\Camera.h"
#include "Layer.h"
#include "Player.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CCamera::CCamera(const CCamera & Prototype)
	: CGameObject( Prototype )
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	CAMERA_DESC*		pDesc = static_cast<CAMERA_DESC*>(pArg);

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	m_fFovy = pDesc->fFovy;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;
	m_eLevelID = pDesc->eLevelID;

	// 플레이어, 카메라 거리
	m_vCameraDist = { 0.f, -5.f, 5.f };
	m_fCameraSpeed = 30.f;

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(m_eLevelID, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

	// 플레이어, 카메라 거리
	m_vCameraDist = { 0.f, -5.f, 5.f };
	m_fCameraSpeed = 30.f;

	m_vWatchPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_vWatch_Camera_Pos = m_vWatchPos - m_vCameraDist;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vWatch_Camera_Pos);
	m_pTransformCom->LookAt(m_vWatchPos);

	return S_OK;
}

_uint CCamera::Priority_Update(_float fTimeDelta)
{
	// Camera_Control();

	MoveTo_Player(fTimeDelta);
	_float3 vWorldMatrix = m_pTransformCom->Get_WorldMatrix();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vWatch_Camera_Pos);
	m_pTransformCom->LookAt(m_vWatchPos);
	
	if (true == m_bShake)
		Camera_Shaking(fTimeDelta);

	/* 장치에 뷰랑 투영행렬을 한번 셋팅 해주면 다시 갱신해 주기 전까지 모든 객체들을 렌더링시에 셋팅해준 행렬을 곱하면서 그리게 된다. */
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_pTransformCom->Get_WorldMatrix_Inverse());
	_float4x4		ProjMatrix{};
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&ProjMatrix, m_fFovy, (_float)g_iWinSizeX / g_iWinSizeY, m_fNear, m_fFar));

	return OBJ_NOEVENT;
}

void CCamera::Update(_float fTimeDelta)
{

}

void CCamera::Late_Update(_float fTimeDelta)
{
	
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Camera_Shaking( _float fTimeDelta)
{
	if (m_fCheckShakeTime < m_fShakeTime)
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		
		if (true == m_bShakeLeft)
			vPos.x -= .05f;
		else
			vPos.x += .05f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_bShakeLeft = !m_bShakeLeft;
	}
	else
	{
		m_fCheckShakeTime = 0.f;
		m_bShake = false;
	}
	m_fCheckShakeTime += fTimeDelta;
}

void CCamera::Camera_Control()
{
	if (m_pGameInstance->Key_Down('K'))
		m_bShake = true;

	if (GetAsyncKeyState('Z'))
	{
		m_fFovy -= 0.01f;

	}
	else if (GetAsyncKeyState('X'))
	{
		m_fFovy += 0.01f;
	}

	if (GetAsyncKeyState('C'))
	{
		m_fFovy  = D3DXToRadian(60.f);
	}
}

HRESULT CCamera::Ready_Components(void* pArg)
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), pArg)))
		return E_FAIL;

	return S_OK;	
}

void CCamera::MoveTo_Player(_float fTimeDelta)
{
	_float3 vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_float3 vDir = vPos - m_vWatchPos;
	_float fLength = D3DXVec3Length(&vDir);

	m_vWatchPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_vWatch_Camera_Pos = m_vWatchPos - m_vCameraDist;
	//cout << "Player : " << m_vWatchPos.x << " " << m_vWatchPos.y << " " << m_vWatchPos.z << endl;
	//cout << "Camera : " << m_vWatch_Camera_Pos.x << " " << m_vWatch_Camera_Pos.y << " " << m_vWatch_Camera_Pos.z << endl;
	//cout << endl;
	//if (fLength < 1.f)
	//{
	// 	m_vWatchPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//	m_vWatch_Camera_Pos = m_vWatchPos - m_vCameraDist;
	//}
	//else
	//{
	//	D3DXVec3Normalize(&vDir, &vDir);
	//	m_vWatchPos += vDir * m_fCameraSpeed * fTimeDelta;
	//	m_vWatch_Camera_Pos += vDir * m_fCameraSpeed * fTimeDelta;
	//}
}

CCamera * CCamera::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera*		pInstance = new CCamera(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CCamera::Clone(void* pArg)
{
	CCamera*		pInstance = new CCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pTransformCom);
}
