#pragma once

#include "Skill.h"


BEGIN(Client)
class CShield final : public CSkill
{
public:
	typedef struct tagGumDesc : public SKILL_DESC
	{
		CTransform* pPlayerTransform;
		_int* pHp;
		_int* pMaxHp;
	}SKILL_SHIELD_DESC;
private:
	CShield(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShield(const CShield& Prototype);
	virtual ~CShield() = default;

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
	 // 지속시간
	_float m_fActiveTime = { 0.f };
	// 총 지속시간
	_float m_fTotalActiveTime = { 0.f };
	// 쉴드량
	_int m_iShiledAmount = { 0 };
	// 플레이어 체력
	_int* m_pHp = { nullptr };
	// 플레이어 최대 체력
	_int* m_pMaxHp = { nullptr };
	// 플레이어 위치
	CTransform* m_pPlayerTransform = { nullptr };
	// 쉴드 딜레이
	_float m_fShieldHitDelay = { 0.f };

	// 블랭궁마냥 전기 떨어지는 거 시간
	_float m_fElectricTime = { 0.f };

public:
	static CShield* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
