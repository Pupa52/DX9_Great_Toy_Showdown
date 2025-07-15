#pragma once

#include "Effect.h"

BEGIN(Client)
class CEffect_Meteor_Explosion final : public CEffect
{
private:
	CEffect_Meteor_Explosion(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Meteor_Explosion(const CEffect_Meteor_Explosion& Prototype);
	virtual ~CEffect_Meteor_Explosion() = default;

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
	static CEffect_Meteor_Explosion* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
