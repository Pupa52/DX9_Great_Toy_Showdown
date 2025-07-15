#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEFFECT_LandMine_Count final : public CEffect
{
private:
	CEFFECT_LandMine_Count(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEFFECT_LandMine_Count(const CEFFECT_LandMine_Count& Prototype);
	virtual ~CEFFECT_LandMine_Count() = default;

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

public:
	static CEFFECT_LandMine_Count* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END