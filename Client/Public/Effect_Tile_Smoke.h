#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Tile_Smoke final : public CEffect
{
private:
	CEffect_Tile_Smoke(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Tile_Smoke(const CEffect_Tile_Smoke& Prototype);
	virtual ~CEffect_Tile_Smoke() = default;

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
	_float3* m_vPos = {};
public:
	static CEffect_Tile_Smoke* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END