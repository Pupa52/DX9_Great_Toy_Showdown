#pragma once

#include "Effect.h"

BEGIN(Client)
class CEffect_Missile_Boom final : public CEffect
{
private:
	CEffect_Missile_Boom(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Missile_Boom(const CEffect_Missile_Boom& Prototype);
	virtual ~CEffect_Missile_Boom() = default;

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
	static CEffect_Missile_Boom* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
