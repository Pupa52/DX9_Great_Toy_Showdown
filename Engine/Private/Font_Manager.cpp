#include "Font_Manager.h"

CFont_Manager::CFont_Manager(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device { pGraphic_Device }
{
	Safe_AddRef(pGraphic_Device);
}

HRESULT CFont_Manager::Initialize()
{
	Load_Font();
	
	for (_uint i = 0; i < 5; i++)
		Create_Font(i);

	Create_Font(180, 40);
	return S_OK;
}

HRESULT CFont_Manager::Draw_Text(_uint iFontSize, const _wstring& strText, RECT& rt, D3DCOLOR tColor)
{
	if (iFontSize >= m_vecFont.size())
		return E_FAIL;

	_uint iResult{};
	iResult = m_vecFont[iFontSize]->DrawText(NULL, strText.c_str(), (_int)strText.length(), &rt, DT_TOP | DT_LEFT | DT_NOCLIP, tColor);

	if (0 == iResult)
		return E_FAIL;

	return S_OK;
}

HRESULT CFont_Manager::Draw_Text(_uint iFontSize, const _wstring& strText, _uint iX, _uint iY, D3DCOLOR tColor)
{
	if (iFontSize >= m_vecFont.size())
		return E_FAIL;

	RECT rt{ (_long)iX, (_long)iY, 0, 0 };

	_uint iResult{};
	iResult = m_vecFont[iFontSize]->DrawText(NULL, strText.c_str(), (_int)strText.length(), &rt, DT_TOP | DT_LEFT | DT_NOCLIP, tColor);

	if (0 == iResult)
		return E_FAIL;

	return S_OK;
}

HRESULT CFont_Manager::Create_Font(_uint iFontSize)
{
	LPD3DXFONT	pFont;
	D3DXFONT_DESC tFontDesc;

	::ZeroMemory(&tFontDesc, sizeof(D3DXFONT_DESC));
	tFontDesc.Height = iFontSize * 10 + 10;
	tFontDesc.Width = 10;
	tFontDesc.MipLevels = 1; // 밉레벨 생성하지 않음 1, 생성함 0
	tFontDesc.Weight = 500;
	tFontDesc.Italic = false;
	tFontDesc.CharSet = HANGUL_CHARSET;
	lstrcpy(tFontDesc.FaceName, L"UhBee Se_hyun Bold");

	if (FAILED(D3DXCreateFontIndirect(m_pGraphic_Device, &tFontDesc, &pFont)))
	{
		MSG_BOX(TEXT("Failed to Created : CFont_Large"));
		return E_FAIL;
	}

	m_vecFont.emplace_back(pFont);

	return S_OK;
}

HRESULT CFont_Manager::Create_Font(_uint iFontHeight, _uint iFontWidth)
{
	LPD3DXFONT	pFont;
	D3DXFONT_DESC tFontDesc;

	::ZeroMemory(&tFontDesc, sizeof(D3DXFONT_DESC));
	tFontDesc.Height = iFontHeight;
	tFontDesc.Width = iFontWidth;
	tFontDesc.MipLevels = 1; // 밉레벨 생성하지 않음 1, 생성함 0
	tFontDesc.Weight = 500;
	tFontDesc.Italic = false;
	tFontDesc.CharSet = HANGUL_CHARSET;
	lstrcpy(tFontDesc.FaceName, L"UhBee Se_hyun Bold");

	if (FAILED(D3DXCreateFontIndirect(m_pGraphic_Device, &tFontDesc, &pFont)))
	{
		MSG_BOX(TEXT("Failed to Created : CFont_Large"));
		return E_FAIL;
	}

	m_vecFont.emplace_back(pFont);

	return S_OK;
}

HRESULT CFont_Manager::Load_Font()
{
	if (0 == AddFontResource(TEXT("../../Client/Resources/Font/GTS.ttf")))
		return E_FAIL;

	if (0 == AddFontResource(TEXT("../../Client/Resources/Font/UhBee Se_hyun Bold.ttf")))
		return E_FAIL;

	return S_OK;
}

CFont_Manager* CFont_Manager::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFont_Manager* pInstance = new CFont_Manager(pGraphic_Device);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CFont_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Manager::Free()
{
	__super::Free();

	RemoveFontResource(TEXT("../../Client/Resources/Font/GTS.ttf"));
	RemoveFontResource(TEXT("../../Client/Resources/Font/UhBee Se_hyun Bold.ttf"));

	for (auto& iter : m_vecFont)
		Safe_Release(iter);

	Safe_Release(m_pGraphic_Device);
}
