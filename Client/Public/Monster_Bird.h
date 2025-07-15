#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonster_Bird final : public CMonster
{
private:
	enum BIRD_STATE {
		BIRD_IDLE,
		BIRD_ACTIVE_START,
		BIRD_ACTIVE,
		BIRD_TELEPORT,
		BIRD_DIE,
		BIRD_END,
		BIRD_PEACE,
	};

private:
	CMonster_Bird(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Bird(const CMonster_Bird& Prototype);
	virtual ~CMonster_Bird() = default;

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
	HRESULT Fire_Bullet(_float fTimeDelta);
	bool TelePort (_float fTimeDelta);
private:
	BIRD_STATE m_eBird_State = BIRD_PEACE;
	vector<_float3> m_vTelePort_Pos = {};
	_float3 Origin_Pos = {};

	_int RandTemp = 0;
	_bool Attack = false;
	_bool Teleport_Spawn = false;
	_bool Teleport_Ready = false;

	_int multi_portal = 1;
public:
	static CMonster_Bird* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END