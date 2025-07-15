#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "GameInstance.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CLobbyCamera final : public CGameObject
{
public:
	typedef struct tagCamera_Desc : public CTransform::TRANSFORM_DESC
	{
		_float3			vEye;
		_float3			vAt;
		_float			fFovy;
		_float			fNear;
		_float			fFar;
	}CAMERA_DESC;

private:
	CLobbyCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLobbyCamera(const CLobbyCamera& Prototype);
	virtual ~CLobbyCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	void Camera_Control();
	HRESULT Ready_Components(void* pArg);
	
	void MoveTo_Character(_float3 vPos, _float3 vLookPos, _float fTimeDelta);

private:
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pCatTransform = { nullptr };
	CTransform* m_pDogTransform = { nullptr };
	CTransform* m_pPenguinTransform = { nullptr };

	_float		m_fFovy = {};
	_float		m_fNear = {};
	_float		m_fFar = {};

	_float3		m_vCameraDist = {};
	_float3		m_vLookCharacterPos = {};
	
	PLAYER_CHARACTER m_eCurrentCharacter = { PLAYER_CHARACTER::PLAYER_END };

public:
	static CLobbyCamera* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END