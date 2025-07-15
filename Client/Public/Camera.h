#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "GameInstance.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CCamera final : public CGameObject
{
public:
	typedef struct tagCamera_Desc : public CTransform::TRANSFORM_DESC
	{
		_float3			vEye;
		_float3			vAt;
		_float			fFovy;
		_float			fNear;
		_float			fFar;
		LEVELID			eLevelID = LEVEL_GAMEPLAY;
	}CAMERA_DESC;

private:
	CCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Shake_Camera(_float fShakeTime) {
		m_fShakeTime = fShakeTime;
		m_bShake = true;
	}

private:
	void Camera_Control();
	HRESULT Ready_Components(void* pArg);
	void MoveTo_Player(_float fTimeDelta);
	void Camera_Shaking(_float fTimeDelta);

private:
	CTransform*	m_pTransformCom = { nullptr };
	CTransform*	m_pPlayerTransform = { nullptr };

	_float		m_fFovy = {};
	_float		m_fNear = {};
	_float		m_fFar = {};

	_float3		m_vCameraDist = {};

	_float3		m_vWatchPos = {};
	_float3		m_vWatch_Camera_Pos = {};
	_float		m_fCameraSpeed = { 0.f };
	

	_float		m_fShakeTime = { 0.f };
	_float		m_fCheckShakeTime = { 0.f };
	_bool		m_bShakeLeft = { false };
	_bool		m_bShake = { false };

	LEVELID		m_eLevelID = { LEVEL_END };

public:
	static CCamera* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END