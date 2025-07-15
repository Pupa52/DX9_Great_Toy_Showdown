#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
END

class CMonster_Fsm : public CGameObject
{
public:
	CMonster_Fsm(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Fsm(const CMonster_Fsm& Prototype);
	virtual ~CMonster_Fsm() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom;
public:
	void Set_Monster_Fsm(MONSTER_STATE* _State);

public:

};

class CMonster