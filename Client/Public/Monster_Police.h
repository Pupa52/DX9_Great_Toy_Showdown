#pragma once

#include "Monster.h"
#include "Monster_Police_Gun.h"


BEGIN(Client)

class CMonster_Police : public CMonster
{
public:
	enum POLICE_STATE
	{
		POLICE_IDLE,
		POLICE_MOVE,
		POLICE_SHOT_READY,
		POLICE_AIM,
		POLICE_SHOT_RETURN,
		POLICE_DIE,
		POLICE_PEACE,
	};
private:
	typedef struct Police_Body_Info
	{
		_float3 fBody_Pos = {};
		_float3 fFrist_Spint_Pos = {};
		_float3 fGun_Pos = {};

	}TPolice_Body_Info;
protected:
	CMonster_Police(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Police(const CMonster_Police& Prototype);
	virtual ~CMonster_Police() = default;

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
public:
	POLICE_STATE Get_Attack_State() { return m_eAttack_State; }
	POLICE_STATE Get_PreAttack_State() { return m_ePre_Attack_State; }
private:
	POLICE_STATE m_eAttack_State = POLICE_PEACE;
	POLICE_STATE m_ePre_Attack_State = POLICE_PEACE;
protected:
	// 2 = ฦศ(?) , รั
	//CGameObject* m_pPolice_Part[2] = { nullptr };
	CGameObject* m_pPolice_Part = { nullptr };

	_bool m_bAttack = false;
	//_bool m_bHasSpine = false;
	_bool m_bHasGun = false;
	_bool m_bCheck = false;

	_float m_dTimeTemp = 0;
	_float m_fTimer = 0;
	_float3 v_pos = {};

	ACTOR_DESC* Act_Desc = {};
public:
	static CMonster_Police* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END