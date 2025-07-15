#pragma once

#include "Effect.h"

BEGIN(Client)
class CEffect_Meteor_Ground final : public CEffect
{
public:
	typedef struct tagGroundDesc : public EFFECT_DESC
	{
		_bool* pDead;
	}METEORGROUND_DESC;

private:
	CEffect_Meteor_Ground(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Meteor_Ground(const CEffect_Meteor_Ground& Prototype);
	virtual ~CEffect_Meteor_Ground() = default;

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
	_bool* m_pDead = { nullptr };
	

public:
	static CEffect_Meteor_Ground* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END