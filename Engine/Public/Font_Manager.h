#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();

public:
	HRESULT Draw_Text(_uint iFontSize, const _wstring& strText, RECT& rt, D3DCOLOR tColor);
	HRESULT Draw_Text(_uint iFontSize, const _wstring& strText, _uint iX, _uint iY, D3DCOLOR tColor);

private:
	HRESULT Load_Font();
	HRESULT Create_Font(_uint iFontSize);
	HRESULT Create_Font(_uint iFontHeight, _uint iFontWidth);

private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = { nullptr };
	vector<LPD3DXFONT>	m_vecFont;

public:
	static CFont_Manager* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END