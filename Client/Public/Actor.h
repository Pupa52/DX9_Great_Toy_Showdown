#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

#include "GameInstance.h"

BEGIN(Client)

class CActor abstract : public CLandObject
{
public:
	typedef struct tagActor_Desc : public CLandObject::LANDOBJECT_DESC
	{
		_int iHp{};
		_int iMaxHp{};

		_uint iAttack{};

		_uint iExp{};
		_uint iMaxExp{};

		_uint iLevel{};
		_float fAngle{};
		_float3 m_Pos{};
		_float3 m_targetPos{};
	}ACTOR_DESC;

protected:
	CActor(LPDIRECT3DDEVICE9 pGraphic_Device);
	CActor(const CActor& Prototype);
	virtual ~CActor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	ACTOR_DESC Get_ActorDesc() const { return m_tActorDesc; };
protected:
	virtual HRESULT Ready_Components() = 0;
	
public:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CAnimator* m_pAnimatorCom = { nullptr };
	CCollider_Cube* m_pColliderCubeCom = { nullptr };

	ACTOR_DESC m_tActorDesc = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END