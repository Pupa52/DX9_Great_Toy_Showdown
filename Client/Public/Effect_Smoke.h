#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Smoke final : public CEffect
{
private:
	CEffect_Smoke(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Smoke(const CEffect_Smoke& Prototype);
	virtual ~CEffect_Smoke() = default;

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
	_int RandPos = 0;
public:
	static CEffect_Smoke* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END