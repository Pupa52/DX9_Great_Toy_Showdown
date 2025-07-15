#pragma once

#include "State.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CTransform;
class CFSM;
class CGameInstance;
END

BEGIN(Client)
class CPlayer_State abstract : public CState
{
protected:
	CPlayer_State(class CPart** pParts, _uint iNumParts, class CTransform* pTransform, 
		_float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
		_int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID);
	virtual ~CPlayer_State() = default;

public:
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT StartState() override;
	HRESULT ExitState() override;

protected:
	HRESULT Fire_Bullet(_float fTimeDelta);
	HRESULT Use_Skill_GumBoom(_wstring strSkillLayerTag);
	HRESULT Use_Skill_Shield(_wstring strSkillLayerTag);
	HRESULT Use_Skill_CarrotBoom(_wstring strSkillLayerTag);

protected:
	Bullet_ID m_eBullet_Id = ID_END;
	class CPart** m_pParts = { nullptr };
	_uint m_iNumParts = { 0 };

	// �÷��̾� ��ġ �޾ƿ��� �뵵.
	class CTransform* m_pTransformCom = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	// �߻� ����
	_float m_fFireLimit = { 0.f };
	_float& m_fFireDelay;
	// �߻� �Ÿ�(�ѱ� ��ġ)
	_float m_fFireDistance = { 0.f };
	// ��ǥ ��ġ(���콺 ��ġ)
	_float3* m_pTargetPos = { nullptr };
	// �÷��̾� ĳ����
	_uint m_iPlayerCharacter = { 0 };
	// ��ų ���
	_bool* m_pUseSkill = { nullptr };
	// ������ ���� ��ų ��ũ
	_uint m_iRank = { 0 };
	// �÷��̾� ü��
	_int* m_pHp = { nullptr };
	// ź��
	_int* m_iBulletCount = { nullptr };
	// ����
	_uint* m_pLevel = { nullptr };
	// ������
	_uint* m_pAttack = { 0 };
	// �ִ� ü��
	_int* m_iMaxHp = { nullptr };
	// ��ų ���̾� �±�
	_int m_iPlayer = { false };
	// ���� ���̵�
	LEVELID m_eLevelID = { LEVEL_END };

public:
	virtual void Free() override;
};
END