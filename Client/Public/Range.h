#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CTexture;
class CVIBuffer_Rect;
class CGameInstance;
END

BEGIN(Client)
class CRange final : public CBlendObject
{
public:
	typedef struct
	{
		CTransform* pTransform;
		PLAYER_CHARACTER ePlayerCharacter;
	}RANGE_DESC;

private:
	CRange(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRange(const CRange& Prototype);
	virtual ~CRange() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	virtual HRESULT Ready_Components();
	HRESULT			SetUp_RenderState();
	HRESULT			Reset_RenderState();

private:
	CTransform* m_pPlayerTransform = { nullptr };

	CTransform* m_pTransformCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	PLAYER_CHARACTER m_ePlayerCharacter = { PLAYER_CHARACTER::PLAYER_END };

public:
	static CRange* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
