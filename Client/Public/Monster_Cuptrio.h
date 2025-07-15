#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonster_Cuptrio final : public CMonster
{
private:
	CMonster_Cuptrio(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Cuptrio(const CMonster_Cuptrio& Prototype);
	virtual ~CMonster_Cuptrio() = default;

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
	
public:
	static CMonster_Cuptrio* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END