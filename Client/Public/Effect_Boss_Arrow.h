#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Boss_Arrow final : public CEffect
{
private:
	CEffect_Boss_Arrow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Boss_Arrow(const CEffect_Boss_Arrow& Prototype);
	virtual ~CEffect_Boss_Arrow() = default;

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
	CTransform* m_pPlayerTransform = { nullptr };
	_bool m_bTurn = true;

	_float Calculator_Arrow();
private:
	_float m_fDelay = 0.f;
	_float3 m_vLastPos = {};
	_bool m_bDrop = true;
	_float m_fAngle = 0.f;

	_int m_iRand = 0;
	_float RandPos = 0;
	_float3 fPos = {};
public:
	static CEffect_Boss_Arrow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END