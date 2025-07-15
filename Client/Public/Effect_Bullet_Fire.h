#pragma once

#include "Effect.h"

BEGIN(Client)
class CEffect_Bullet_Fire final : public CEffect
{
private:
	CEffect_Bullet_Fire(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Bullet_Fire(const CEffect_Bullet_Fire& Prototype);
	virtual ~CEffect_Bullet_Fire() = default;

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
	static CEffect_Bullet_Fire* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
