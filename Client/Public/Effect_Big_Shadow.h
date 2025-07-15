#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Big_Shadow final : public CEffect
{
private:
	CEffect_Big_Shadow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Big_Shadow(const CEffect_Big_Shadow& Prototype);
	virtual ~CEffect_Big_Shadow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Find_Obj();
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
private:
	_float m_fTimer = 0.f;
	_bool m_bPlayer = false;
	_bool m_bShadow = false;
	_float3* m_pos = {};
	_int RandPos = 0;
public:
	static CEffect_Big_Shadow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END