#pragma once
#include "Skill.h"

class CElectric_Bolt final : public CSkill
{
private:
	CElectric_Bolt(LPDIRECT3DDEVICE9 pGraphic_Device);
	CElectric_Bolt(const CElectric_Bolt& Prototype);
	virtual ~CElectric_Bolt() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);


private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	_float m_fTextureIndes = { 0 };
	_uint m_iFrameCount = { 0 };
	_bool m_bDead = { false };
	_bool m_bMonsterHit = { false };
	_bool m_bHit = { false };

public:
	static CElectric_Bolt* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

