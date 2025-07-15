#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)
class CPart abstract : public CBase
{
public:
	typedef struct
	{
		CTransform* pTransformCom;
		CVIBuffer_Rect*	pVIBufferCom;
	}PART_DESC;

protected:
	CPart(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CPart() = default;

public:
	virtual HRESULT Initialize(_uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID);
	virtual void Priority_Update(_float fTimeDelta) ;
	virtual void Update(_float fTimeDelta) ;
	virtual void Late_Update(_float fTimeDelta) ;
	virtual HRESULT Render() ;

public:
	void			Set_Dir(PLAYER_DIR eDir) { m_eCurDir = eDir; }
	void			Set_Angle(_float fAngle) { m_fAngle = fAngle; }
	_float			Get_Angle() { return m_fAngle; }
	_bool			Dir_Changed();
	virtual HRESULT Set_Animation_Attack() = 0;
	virtual HRESULT Set_Animation_Idle() = 0;
	virtual HRESULT Set_Animation_Move() = 0;

protected:
	virtual HRESULT Ready_Components(_uint iLevelIndex);
	void			Inverse_Vertex();

	HRESULT			SetUp_RenderState();
	HRESULT			Reset_RenderState();

protected:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	
	// �÷��̾� ��ȯ ����
	CTransform*	m_pTransformCom = { nullptr };
	// �÷��̾� ���� ����
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	// �ؽ�ó ����
	CAnimator* m_pAnimatorCom = { nullptr };
	// ���� ������ ��ȯ
	CTransform* m_pTransformPart = { nullptr };

	// ���� �� �׸��� �켱 ����
	_float3 m_fDist = {};

	// �÷��̾� ����
	PLAYER_DIR m_eCurDir = { PLAYER_DIR::DIR_END };
	PLAYER_DIR m_ePreDir = { PLAYER_DIR::DIR_END };

	// �÷��̾� ��
	PLAYER_CHARACTER m_ePlayerID = { PLAYER_CHARACTER::PLAYER_END };

	_float m_fAngle = { 0.f };

	_bool m_bInverse = { false };

public:
	virtual void Free() override;
};

END
