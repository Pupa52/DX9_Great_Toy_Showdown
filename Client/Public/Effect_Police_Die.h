#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Police_Die final : public CEffect
{
private:
	CEffect_Police_Die(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Police_Die(const CEffect_Police_Die& Prototype);
	virtual ~CEffect_Police_Die() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
private:
	_bool m_bGround = false;

	_float m_fAirTime = 0.f;
	_float m_fGravity = 9.8f;
	_float3 m_vPos = {};
public:
	static CEffect_Police_Die* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END