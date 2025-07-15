#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Level_Up_Box final : public CEffect
{
private:
	CEffect_Level_Up_Box(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Level_Up_Box(const CEffect_Level_Up_Box& Prototype);
	virtual ~CEffect_Level_Up_Box() = default;

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
	_int m_iRand = 0;
	_float3 fPos = {};
public:
	static CEffect_Level_Up_Box* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END