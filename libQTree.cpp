#include "libQTree.h"
template< class T>
IKD::OBJECT_FOR_TREE<T>::OBJECT_FOR_TREE(int id = 0) {
	m_id = id;
	m_pCell = NULL;
	m_pObject = NULL;
	m_pPre = NULL;
	m_pNext = NULL;
}
template< class T>
 bool IKD::OBJECT_FOR_TREE<T>::Remove() {
	{
		// ���łɗ��E���Ă���(=�o�^��Ԃ�����)���͏����I��
		if (!m_pCell)
			return false;

		// ������o�^���Ă����ԂɎ��g��ʒm
		if (!m_pCell->OnRemove(this))
			return false;

		// ���E����
		// �O��̃I�u�W�F�N�g�����т���(unlink)  A<->B<->C��B��������Ƃ�A<->C�ɂ�����
		if (m_pPre != 0)
		{
			m_pPre->m_pNext = m_pNext;
		}
		if (m_pNext != 0)
		{
			m_pNext->m_pPre = m_pPre;
		}
		m_pPre = 0;
		m_pNext = 0;
		m_pCell = NULL;
		return true;
	}
}