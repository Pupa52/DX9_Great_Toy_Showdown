#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonster_Magic_mirror final : public CMonster
{
private:
	enum Mirror_State {
		Mirror_State_Back_to_Front,
		Mirror_State_Idle,
		Mirror_State_Die,
	};
private:
	CMonster_Magic_mirror(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Magic_mirror(const CMonster_Magic_mirror& Prototype);
	virtual ~CMonster_Magic_mirror() = default;

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
	void Player_Stalking(_float fTimeDelta);
private:
	Mirror_State m_eState = Mirror_State_Idle;
	bool m_bCanMove = false;

	CTransform::TRANSFORM_DESC		TransformDesc{};

	_float m_fLifeTime = 0;
	_float m_fTimeTemp = 0;
	_float	m_foverlapTime = 0;
	_float3 Back_Temp = {};
public:
	static CMonster_Magic_mirror* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END