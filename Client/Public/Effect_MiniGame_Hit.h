#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_MiniGame_Hit final : public CEffect
{
private:
	CEffect_MiniGame_Hit(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_MiniGame_Hit(const CEffect_MiniGame_Hit& Prototype);
	virtual ~CEffect_MiniGame_Hit() = default;

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
	_float RandPos = 0;
	_float3 fPos = {};
public:
	static CEffect_MiniGame_Hit* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END