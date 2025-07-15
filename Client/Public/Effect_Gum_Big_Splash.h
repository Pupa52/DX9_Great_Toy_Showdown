#pragma once

#include "Effect.h"

BEGIN(Client)
class CEffect_Gum_Big_Splash final : public CEffect
{
private:
	CEffect_Gum_Big_Splash(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Gum_Big_Splash(const CEffect_Gum_Big_Splash& Prototype);
	virtual ~CEffect_Gum_Big_Splash() = default;

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
	static CEffect_Gum_Big_Splash* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
