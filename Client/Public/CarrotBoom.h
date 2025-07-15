#pragma once

#include "Skill.h"


BEGIN(Client)
class CCarrotBoom final : public CSkill
{
public:
	typedef struct tagGumDesc : public SKILL_DESC
	{
	}SKILL_CARROT_DESC;
private:
	CCarrotBoom(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCarrotBoom(const CCarrotBoom& Prototype);
	virtual ~CCarrotBoom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

	

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	_float Cal_Angle();

private:
	// ����ü �ӵ�
	_float m_fSpeed = { 0.f };
	// ������
	_int m_iDamage = { 0 };
	// ��ü ����
	_bool m_bDead = { false };
	// ������ �Ÿ� -> ���� �Ÿ� ������ ����
	_float m_fMoveLength = { 0.f };
	
	// ������ ��
	_float m_fForce = { 0.f };
	// ���� ù �� ����
	_float m_fFistForce = { 0.f };

public:
	static CCarrotBoom* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
END
