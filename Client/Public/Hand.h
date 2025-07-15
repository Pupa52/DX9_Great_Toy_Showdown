#pragma once

#include "Part.h"

BEGIN(Client)
class CHand final : public CPart
{
private:
	CHand(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CHand() = default;

public:
	virtual HRESULT Initialize(_uint iLevelIndex, void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	// CPart을(를) 통해 상속됨
	HRESULT Set_Animation_Attack() override;
	HRESULT Set_Animation_Idle() override;
	HRESULT Set_Animation_Walk() override;
private:
	virtual HRESULT Ready_Components(_uint iLevelIndex);
	
public:
	static CHand* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;

};
END
