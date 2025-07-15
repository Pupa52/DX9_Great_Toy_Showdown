#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonster_Police_First_Spine : public CMonster
{
private:
	CMonster_Police_First_Spine(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Police_First_Spine(const CMonster_Police_First_Spine& Prototype);
	virtual ~CMonster_Police_First_Spine() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
private:
	_bool m_b_hasBody = false;
public:
	static CMonster_Police_First_Spine* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END