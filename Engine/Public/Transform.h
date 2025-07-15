#pragma once

#include "Component.h"

/* ��ü�� ����(���彺���̽� ����)�� ������ �ִ´�. */
/* ���� : ũ��, ȸ��, ��ġ -> WorldMatrix */
/* WorldMatrix�� ���ؼ� ���� �����ȿ����� �پ��� ����� �����Ѵ�. */
/* WorldMatrix -> ���ý����̽��� �����ϴ� ������ ��ġ�� ���� �����̽� ��ȯ�ϳ�. */
/* -> ��������� �� ��(Right, Up, Look, Position)�� ���彺���̽������� ��ü������ �ǹ��Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE {STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransform_Desc
	{
		_float		fSpeedPerSec{};
		_float		fRotationPerSec{};
	}TRANSFORM_DESC;

private:
	CTransform(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;

public:
	_float3 Get_State(STATE eState) {
		return *(_float3*)&m_WorldMatrix.m[eState][0];
	}

	_float3 Get_Scaled() {
		return _float3(D3DXVec3Length(&Get_State(STATE_RIGHT)),
			D3DXVec3Length(&Get_State(STATE_UP)),
			D3DXVec3Length(&Get_State(STATE_LOOK)));
	}

	_float4x4	Get_WorldMatrix() const {
		return m_WorldMatrix;
	}

	_float4x4 Get_WorldMatrix_Inverse() {
		_float4x4	WorldMatrixInverse;
		return *D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &m_WorldMatrix);
	}

	void Set_State(STATE eState, const _float3& vState) {
		memcpy(&m_WorldMatrix.m[eState][0], &vState, sizeof(_float3));
	}

	void Set_WorldMatrix(const _float4x4 _WorldMatrix){
		m_WorldMatrix = _WorldMatrix;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Transform();

public:
	void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Turn(const _float3& vAxis, _float fTimeDelta);
	void Rotation(const _float3& vAxis, _float fRadian);
	void LookAt(const _float3& vAt);
	void LookAt_ForLandObject(const _float3& vAt);


private:
	_float4x4			m_WorldMatrix = {};

	_float				m_fSpeedPerSec = {};
	_float				m_fRotationPerSec = {};

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END