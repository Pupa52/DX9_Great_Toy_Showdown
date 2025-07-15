#pragma once

#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CAnimator final : public CComponent
{
protected:
	CAnimator(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAnimator(const CAnimator& Prototype);
	virtual ~CAnimator() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Add_Texture(const _wstring& strFrameKey, CTexture* pTextures);
	HRESULT Bind_Texture();
	HRESULT Bind_Second_Texture();
	HRESULT Change_Texture(const _wstring& strFrameKey);
	HRESULT Move_Frame(_float fTimeDelta);

private:
	map<const _wstring, CTexture*>	m_pAnimations;
	CTexture*						m_pCurTexture = { nullptr };
	const _wstring					m_strFrameKey = { };

public:
	static CAnimator* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END