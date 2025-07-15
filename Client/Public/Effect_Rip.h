#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Rip final : public CEffect
{
private:
	CEffect_Rip(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Rip(const CEffect_Rip& Prototype);
	virtual ~CEffect_Rip() = default;

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
	_bool m_bDrop = true;

	_int m_iRand = 0;
	_float RandPos = 0;
	_float3 fPos = {};
public:
	static CEffect_Rip* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END