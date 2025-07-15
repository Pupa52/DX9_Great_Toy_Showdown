#pragma once

#include "Part.h"

BEGIN(Client)
class CHead final : public CPart
{
private:
	CHead(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CHead() = default;

public:
	virtual HRESULT Initialize(_uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Set_Animation_Attack() override;
	HRESULT Set_Animation_Idle() override;
	HRESULT Set_Animation_Move() override;

private:
	virtual HRESULT Ready_Components(_uint iLevelIndex);


public:
	static CHead* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID);
	virtual void Free() override;

};
END
