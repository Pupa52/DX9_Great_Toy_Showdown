#include "../Public/Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize(_uint iMaxChannel)
{
	if (FMOD_OK != System_Create(&m_pSystem))
		return E_FAIL;

	if (FMOD_OK != m_pSystem->getVersion(&m_iVersion))
		return E_FAIL;

	if (FMOD_OK != m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr))
		return E_FAIL;

	m_vecChannel.resize(iMaxChannel, nullptr);

	LoadSoundFile();

	return S_OK;
}

void CSound_Manager::SoundPlay(TCHAR* pSoundKey, _uint iChannelID, _float fVolume)
{
	auto iter = m_mapSound.find(pSoundKey);
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	_bool isPlaying = false;

	if (m_vecChannel[iChannelID]->isPlaying(&isPlaying))
	{
		m_vecChannel[iChannelID]->stop();
		m_pSystem->playSound(iter->second, 0, false, &m_vecChannel[iChannelID]);
	}

	m_vecChannel[iChannelID]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::PlayBGM(TCHAR* pSoundKey, _uint iBGMChannel, _float fVolume)
{
	auto iter = m_mapSound.find(pSoundKey);
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});
	StopSound(iBGMChannel);
	m_pSystem->playSound(iter->second, 0, false, &m_vecChannel[iBGMChannel]);
	m_vecChannel[iBGMChannel]->setMode(FMOD_LOOP_NORMAL);
	m_vecChannel[iBGMChannel]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::StopSound(_uint iChannelID)
{
	m_vecChannel[iChannelID]->stop();
}

void CSound_Manager::StopAll()
{
	for (auto iter : m_vecChannel)
		iter->stop();
}

void CSound_Manager::SetChannelVolume(_uint iChannelID, _float fVolume)
{
	m_vecChannel[iChannelID]->setVolume(fVolume);
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	_finddatai64_t fd;

	// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
	intptr_t handle = _findfirsti64("../../Client/Resources/Sound/*.wav", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	// D:\����ȯ\145��\DefaultWindow\Sound	: ���� ���

	char szCurPath[128] = "../../Client/Resources/Sound/";	 // ��� ���
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/" + "Success.wav"
		strcat_s(szFullPath, fd.name);
		// "../Sound/Success.wav"

		Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = (int)strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// �ƽ�Ű �ڵ� ���ڿ��� �����ڵ� ���ڿ��� ��ȯ�����ִ� �Լ�
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
		iResult = _findnexti64(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}

CSound_Manager* CSound_Manager::Create(_uint iMaxChannel)
{
	CSound_Manager* pInstance = new CSound_Manager();
	if (FAILED(pInstance->Initialize(iMaxChannel)))
	{
		MSG_BOX(TEXT("Failed to Created : CSound_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	m_vecChannel.clear();

	for (auto& MyPair : m_mapSound)
	{
		delete[] MyPair.first;
		MyPair.second->release();
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
