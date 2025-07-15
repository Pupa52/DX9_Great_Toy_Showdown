#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonster_Heart final : public CMonster
{
private:
	CMonster_Heart(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Heart(const CMonster_Heart& Prototype);
	virtual ~CMonster_Heart() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);
	HRESULT Find_Enemy();
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
private:
	CTransform::TRANSFORM_DESC		TransformDesc{};

	_float Calculate_Range = 5.f;
	_float m_dTimeTemp = 0;
	_float	m_foverlapTime = 0;

	_float3 Back_Temp = {};
	_float3 m_fEnemyDistance = {};
	_float3 m_vTargetPos = {};
public:
	static CMonster_Heart* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END