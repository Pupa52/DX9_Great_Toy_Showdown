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

	 // 지속시간
	_float m_fActiveTime = { 0.f };
	// 중력
	_float m_fGravity = { 0.f };
	// 던지는 힘
	_float m_fForce = { 0.f };
	// 제일 첫 힘 저장
	_float m_fFistForce = { 0.f };
	// 투사체 속도
	_float m_fSpeed = { 0.f };
	// 발사 위치
	_float3 m_vStartPos = {  };
	// 물방울 튀는 빈도
	_float m_fSplashTime = { 0.f };
	// 데미지 간격
	_float m_fDamageDelay = { 0.f };
	
public:
	static CGumBoom* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
END
