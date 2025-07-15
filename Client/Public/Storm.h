#pragma once

#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cylinder;
END

BEGIN(Client)
class CStorm final : public CBlendObject
{
private:
	CStorm(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStorm(const CStorm& Prototype);
	virtual ~CStorm() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float Get_Radius() { return m_fRadius; }

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	CTexture*			m_pTextureCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Cylinder* m_pVIBufferCom = { nullptr };

	_float				m_fRadius = { 0.f };
	_float				m_fHeight = { 0.f };

public:
	static CStorm* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CStorm* Clone(void* pArg);
	virtual void Free() override;
};
END