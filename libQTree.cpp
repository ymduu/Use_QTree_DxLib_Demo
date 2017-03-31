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
		// すでに離脱している(=登録空間が無い)時は処理終了
		if (!m_pCell)
			return false;

		// 自分を登録している空間に自身を通知
		if (!m_pCell->OnRemove(this))
			return false;

		// 離脱処理
		// 前後のオブジェクトを結びつける(unlink)  A<->B<->CでBが抜けるときA<->Cにしたい
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