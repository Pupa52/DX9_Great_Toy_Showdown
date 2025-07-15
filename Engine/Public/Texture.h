#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TYPE { TYPE_TEXTURE2D/* : LPDIRECT3DTEXTURE9*/, TYPE_TEXTURECUBE/* : LPDIRECT3DCUBETEXTURE9*/, TYPE_END };
	typedef struct tagFrame {
		_float fFrameStart;
		_float fFrameCount;
		_float fFrameEnd;
	}FRAME;

private:
	CTexture(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eTextureType, const _wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Texture(_uint iIndex); // 원본
	HRESULT Bind_Texture(); // 애니메이션 용
	HRESULT Bind_Second_Texture(_uint iIndex); // 원본 두번째텍스쳐
	HRESULT Bind_Second_Texture(); // 애니메이션 두번째 용

	HRESULT Move_Frame(_float fDeltaTime);
	
 	void Reset_Frame() { m_tFrame.fFrameCount = m_tFrame.fFrameStart; };

private:
	FRAME									m_tFrame = { };
	_uint									m_iNumTextures = { 0 };
	vector<IDirect3DBaseTexture9*>			m_Textures;

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eTextureType, const _wstring& strTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END