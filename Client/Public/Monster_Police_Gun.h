	#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonster_Police_Gun final : public CMonster
{
private:
	CMonster_Police_Gun(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Police_Gun(const CMonster_Police_Gun& Prototype);
	virtual ~CMonster_Police_Gun() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	void Set_pMonster(CGameObject* _pMonster) { m_pMonster = _pMonster; }
	void Set_ChangeAnim(_bool _bCheck) { m_bCheck = _bCheck; }
	//void Set_Animator(const _wstring& anim_string) { m_pAnimatorCom->Change_Texture(anim_string); }	
private:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
private:
	_float Gun_Angle();
private:
	_float m_fLastAngle = 0.f;

	_bool m_bAttack = false;
	_bool m_bCheck = false;
	// °æÂû

	CGameObject* m_pMonster = { nullptr };
public:
	static CMonster_Police_Gun* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END