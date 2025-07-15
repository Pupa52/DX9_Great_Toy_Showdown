#pragma once

#include "Effect.h"

BEGIN(Client)
class CEffect_Gum_Skul final : public CEffect
{
private:
	CEffect_Gum_Skul(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Gum_Skul(const CEffect_Gum_Skul& Prototype);
	virtual ~CEffect_Gum_Skul() = default;

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
	static CEffect_Gum_Skul* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
