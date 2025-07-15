#pragma once

#include "UI.h"

BEGIN(Client)

class CLoading_Pattern final : public CUI
{
public:
	typedef struct tagPatternDesc
	{
		_float fX;
		_float fY;
		_bool bUp;
	}PATTERN_DESC;

private:
	CLoading_Pattern(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLoading_Pattern(const CLoading_Pattern& Prototype);
	virtual ~CLoading_Pattern() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components();
	virtual HRESULT SetUp_RenderState();
	virtual HRESULT Reset_RenderState();

private:
	_bool	m_bUp = { false };

public:
	static CLoading_Pattern* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END