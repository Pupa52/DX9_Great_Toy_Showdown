#pragma once

#include "Skill.h"


BEGIN(Client)
class CGumBoom final : public CSkill
{
public:
	typedef struct tagGumDesc : public SKILL_DESC
	{
	}SKILL_GUM_DESC;
private:
	CGumBoom(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGumBoom(const CGumBoom& Prototype);
	virtual ~CGumBoom() = default;

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

private:

	 // ���ӽð�
	_float m_fActiveTime = { 0.f };
	// �߷�
	_float m_fGravity = { 0.f };
	// ������ ��
	_float m_fForce = { 0.f };
	// ���� ù �� ����
	_float m_fFistForce = { 0.f };
	// ����ü �ӵ�
	_float m_fSpeed = { 0.f };
	// �߻� ��ġ
	_float3 m_vStartPos = {  };
	// ����� Ƣ�� ��
	_float m_fSplashTime = { 0.f };
	// ������ ����
	_float m_fDamageDelay = { 0.f };
	
public:
	static CGumBoom* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
END
