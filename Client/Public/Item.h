#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END

class CItem abstract : public CLandObject
{
public:
	enum ITEM_TYPE { ITEM_SWORD, ITEM_SKATE, ITEM_MAGAZINE, ITEM_MISSILE, ITEM_METEOR, ITEM_MINE, ITEM_TELEPORT, ITEM_EXP , ITEM_END };

	typedef struct tagItem_Desc : public CLandObject::LANDOBJECT_DESC
	{
		ITEM_TYPE eItemType;
		_float3 vPos;
	}ITEM_DESC;

protected:
	CItem(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem(const CItem& Prototype);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Ready_Component() = 0;
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();


protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };

	// 움직임 관련
	_float		m_fOffset = { 0.f };
	_float		m_fMoveSpeed = { 0.f };
	_float		m_fMoveTime = { 0.f };
	_bool		m_bUpOrDown = { false };

	// 메모리 해제
	_bool m_bDead = { false };

private:
	void Move_OffSet_Updown(_float fTimeDelta);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

