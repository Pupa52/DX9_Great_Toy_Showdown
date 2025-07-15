#include "Key_Manager.h"

CKey_Manager::CKey_Manager()
{
}

HRESULT CKey_Manager::Initialize()
{
    for (_int i = 0; i < KB_END; i++)
        ::ZeroMemory(m_bKeyState, sizeof(bool) * VK_MAX);

    return S_OK;
}

_bool CKey_Manager::Key_Pressing(int _iKey, KEYBUFFER keyBuffer)
{
    if (GetAsyncKeyState(_iKey) & 0x8000)
        return true;

    return false;
}

_bool CKey_Manager::Key_Down(int _iKey, KEYBUFFER keyBuffer)
{
    // ������ ���� ���� ����, ���� ���� ����

    if ((!m_bKeyState[keyBuffer][_iKey]) && (GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyState[keyBuffer][_iKey] = !m_bKeyState[_iKey];
        return true;
    }
    else
    {
        m_bKeyState[keyBuffer][_iKey] = GetAsyncKeyState(_iKey);
    }
    return false;
}

_bool CKey_Manager::Key_Up(int _iKey, KEYBUFFER keyBuffer)
{
    // ������ ���� ���� �ְ�, ����� ������ ���� ����

    if ((m_bKeyState[keyBuffer][_iKey]) && !(GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyState[keyBuffer][_iKey] = !m_bKeyState[_iKey];
        return true;
    }

    // key ���� ����
    for (int i = 0; i < VK_MAX; ++i)
    {
        if ((!m_bKeyState[keyBuffer][i]) && (GetAsyncKeyState(_iKey) & 0x8000))
            m_bKeyState[keyBuffer][i] = !m_bKeyState[keyBuffer][i];
    }

    return false;
}

CKey_Manager* CKey_Manager::Create()
{
    CKey_Manager* pInstance = new CKey_Manager();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CKey_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKey_Manager::Free()
{
    __super::Free();
}
