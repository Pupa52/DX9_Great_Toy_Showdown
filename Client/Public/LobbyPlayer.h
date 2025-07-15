#pragma once
#include "Actor.h"

BEGIN(Client)
class CLobbyPlayer final : public CActor
{
public:
	typedef struct tagLobbyPlayerDesc : public ACTOR_DESC
	{
		PLAYER_CHARACTER ePlayerCharacter;
	}LOBBYPlAYER_DESC;

private:
	enum LOBBYPLAYER_STATE { CHANGE, IDLE, LOBBYSTATE_END };

private:
	CLobbyPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLobbyPlayer(const CLobbyPlayer& Prototype);
	virtual ~CLobbyPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	ACTOR_DESC Get_ActorDesc() const { return m_tActorDesc; };


private:
	virtual HRESULT Ready_Components() override;
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	CAnimator* m_pGunAnimatorCom = { nullptr };
	
	PLAYER_CHARACTER m_ePlayerCharacter = { PLAYER_CHARACTER::PLAYER_END };
	LOBBYPLAYER_STATE m_ePlayerState = { LOBBYSTATE_END };

public:
	static CLobbyPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END


