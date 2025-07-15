#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Scanner_Laser final : public CEffect
{
private:
	CEffect_Scanner_Laser(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Scanner_Laser(const CEffect_Scanner_Laser& Prototype);
	virtual ~CEffect_Scanner_Laser() = default;

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
	_int m_iRotation = 0;
	_float m_fTimer = 0.f;
	_float m_fAngle = 0.f;

	_int m_iRand = 0;
	_float RandPos = 0;
	_float3 fPos = {};
public:
	static CEffect_Scanner_Laser* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END