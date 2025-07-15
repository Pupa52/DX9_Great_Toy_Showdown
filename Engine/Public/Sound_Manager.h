#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize(_uint iMaxChannel);

public:
	void SoundPlay(TCHAR* pSoundKey, _uint iChannelID, _float fVolume);
	void PlayBGM(TCHAR* pSoundKey, _uint iBGMChannel, _float fVolume);
	void StopSound(_uint iChannelID);
	void StopAll();
	void SetChannelVolume(_uint iChannelID, _float fVolume);
	void LoadSoundFile();

private:
	// ���� ���ҽ� ������ ���� ��ü
	map<TCHAR*, Sound*>	m_mapSound;

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	System*				m_pSystem = { nullptr };

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	vector<Channel*>	m_vecChannel;
	_uint				m_iVersion = {};

public:
	static CSound_Manager* Create(_uint iMaxChannel);
	virtual void Free() override;
};

END