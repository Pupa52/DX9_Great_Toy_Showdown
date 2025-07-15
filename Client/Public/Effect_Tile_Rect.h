#pragma once

#include "Effect.h"

BEGIN(Client)

class CEffect_Tile_Rect final : public CEffect
{
private:
	CEffect_Tile_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Tile_Rect(const CEffect_Tile_Rect& Prototype);
	virtual ~CEffect_Tile_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float3	Get_EffectPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	void Set_EffectPos(_float3 _SetPos) { m_pTransformCom->Set_State(CTransform::STATE_POSITION, _SetPos); }

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
private:
	_int m_iSpriteCount = 1;
public:
	static CEffect_Tile_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END