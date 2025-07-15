#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonster_Telephone final : public CMonster
{
private:

	enum Telephone_Attack_State
	{
		Telephone_Idle,
		Telephone_Attack,
		Telephone_Attack_LandMine,
		Telephone_Attack_Drop_Telephone,
		Telephone_Attack_DIE,
		Telephone_Attack_END,
	};
private:
	CMonster_Telephone(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Telephone(const CMonster_Telephone& Prototype);
	virtual ~CMonster_Telephone() = default;

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
	_float m_fFireTime = 0.f;
	_bool m_bFire = false;
	_float m_fTimer = 0;
	Telephone_Attack_State m_eAttack_State = Telephone_Idle;
	bool m_bAttack = false;
	vector<_float3> v_pos = {};
public:
	static CMonster_Telephone* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END