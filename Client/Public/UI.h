#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI abstract : public CGameObject
{
public:
	typedef struct tagUIDesc
	{
		_float fSizeX;
		_float fSizeY;
		_float fX;
		_float fY;
		CGameObject* pOwner;
		_int	iType;
		CUI*	pUI;
		_int	iNum;
	}UI_DESC;

protected:
	CUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI(const CUI& Prototype);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_UI_Dead() { m_bDead = true; }

protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
	/* 어떤 위치(fX, fY)에 어떤 사이즈(fSizeX, fSizeY)로 그릴건지를 설정한 데이터다.  */
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;
	_bool					m_bDead = { false };
	CTransform*				m_pOwnerTransformCom = { nullptr };

protected:
	UI_DESC					m_tUIDesc = {};

protected:
	virtual HRESULT Ready_Components() = 0;
	virtual HRESULT SetUp_RenderState();
	virtual HRESULT Reset_RenderState();

public:
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END