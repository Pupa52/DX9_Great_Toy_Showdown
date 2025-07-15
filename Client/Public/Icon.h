#pragma once

#include "UI.h"

BEGIN(Client)

class CIcon final : public CUI
{
public:
	enum class ICON_TYPE { IT_HEALTH, IT_POWER, IT_BULLET, IT_MOUSE, IT_MARK, IT_END };

private:
	CIcon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CIcon(const CIcon& Prototype);
	virtual ~CIcon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components();
private:
	ICON_TYPE	m_eType = { ICON_TYPE::IT_END };

public:
	static CIcon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END