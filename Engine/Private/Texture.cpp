#include "..\Public\Texture.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CTexture::CTexture(const CTexture& Prototype)
	: CComponent{ Prototype }
	, m_iNumTextures{ Prototype.m_iNumTextures }
	, m_Textures{ Prototype.m_Textures }
	, m_tFrame{ Prototype.m_tFrame }
{
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);
}

HRESULT CTexture::Initialize_Prototype(TYPE eTextureType, const _wstring& strTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	m_Textures.reserve(iNumTextures);

	for (size_t i = 0; i < iNumTextures; i++)
	{
		IDirect3DBaseTexture9* pTexture = { nullptr };

		_tchar		szTextureFilePath[MAX_PATH] = TEXT("");

		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);

		HRESULT hr = eTextureType == TYPE_TEXTURE2D
			? D3DXCreateTextureFromFile(m_pGraphic_Device, szTextureFilePath, reinterpret_cast<LPDIRECT3DTEXTURE9*>(&pTexture))
			: D3DXCreateCubeTextureFromFile(m_pGraphic_Device, szTextureFilePath, reinterpret_cast<LPDIRECT3DCUBETEXTURE9*>(&pTexture));

		if (FAILED(hr))
			return E_FAIL;

		m_Textures.emplace_back(pTexture);
	}

	m_tFrame.fFrameStart = m_tFrame.fFrameCount = 0.f;
	m_tFrame.fFrameEnd = (_float)m_iNumTextures;

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_Texture(_uint iIndex)
{
	if (iIndex >= m_iNumTextures)
		return E_FAIL;

	return m_pGraphic_Device->SetTexture(0, m_Textures[iIndex]);
}

HRESULT CTexture::Bind_Texture()
{
	if (m_tFrame.fFrameCount > m_tFrame.fFrameEnd)
		return E_FAIL;

	return m_pGraphic_Device->SetTexture(0, m_Textures[_uint(m_tFrame.fFrameCount)]);
}

HRESULT CTexture::Bind_Second_Texture(_uint iIndex) // 원본 두번째텍스쳐
{
	if (iIndex >= m_iNumTextures)
		return E_FAIL;

	return m_pGraphic_Device->SetTexture(1, m_Textures[iIndex]);
}

HRESULT CTexture::Bind_Second_Texture() // 애니메이터 두번째
{
	if (m_tFrame.fFrameCount > m_tFrame.fFrameEnd)
		return E_FAIL;

	return m_pGraphic_Device->SetTexture(1, m_Textures[_uint(m_tFrame.fFrameCount)]);
}

HRESULT CTexture::Move_Frame(_float fDeltaTime)
{
	m_tFrame.fFrameCount += m_tFrame.fFrameEnd * fDeltaTime;

	if (m_tFrame.fFrameCount > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrameCount = m_tFrame.fFrameStart;
		return S_OK;
	}
	return E_FAIL;
}

CTexture* CTexture::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eTextureType, const _wstring& strTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(eTextureType, strTextureFilePath, iNumTextures)))
	{
         		MSG_BOX(TEXT("Failed to Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);

	m_Textures.clear();
}
