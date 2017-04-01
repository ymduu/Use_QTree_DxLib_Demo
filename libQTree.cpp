#include "libQTree.h"


template< class T>
void IKD::CCell<T>::ResetLink(OBJECT_FOR_TREE<T> *pOFT) {
	//�ċA�ŃI�u�W�F�N�g���X�g���Ō�܂ŒH���āA�Ōォ�珇�ԂɃ|�C���^Next��0�Œׂ�
	if (pOFT->m_pNext != 0) {
		ResetLink(pOFT->m_pNext);
	}
	pOFT = 0;
}
template< class T>
bool IKD::CCell<T>::Push(OBJECT_FOR_TREE<T> *pOFT) {
	if (pOFT == 0) return false;	// �����I�u�W�F�N�g�͓o�^���Ȃ�
	if (pOFT->m_pCell == this) return false;	// 2�d�o�^�`�F�b�N�A���łɂ��̋�Ԃɓo�^����Ă���Ȃ�o�^���Ȃ�
	if (m_pLatest == 0) {
		m_pLatest = pOFT;	// ��ԂɐV�K�o�^
	}
	else
	{
		// �ŐVOFT�I�u�W�F�N�g���X�V
		pOFT->m_pNext = m_pLatest;		//���ǉ�����I�u�W�F�N�g�̎��̃I�u�W�F�N�g�Ƃ��č��̍ŐV�I�u�W�F�N�g��ݒ�
		m_pLatest->m_pPre = pOFT;		//���̍ŐV�̑O�̃I�u�W�F�N�g�Ƃ��č���ǉ�����I�u�W�F�N�g��ݒ�
		m_pLatest = pOFT;				//�ŐV�I�u�W�F�N�g�͍��ǉ��������̂�
	}
	pOFT->m_pCell = this;	// ��Ԃ�o�^
	return true;
}



template<class T>
bool IKD::CCell<T>::OnRemove(OBJECT_FOR_TREE<T> *pRemoveObj)
{
	if (m_pLatest == pRemoveObj) {
		// ���̃I�u�W�F�N�g�ɑ}���ւ�
		if (m_pLatest != NULL)
			m_pLatest = m_pLatest->m_pNext;
	}
	return true;
}


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

	template< class T>
	IKD::CLiner4TreeManager<T>::CLiner4TreeManager(){
		m_uiLevel = 0;
		m_fW = 0.0f;
		m_fH = 0.0f;
		m_fLeft = 0.0f;
		m_fTop = 0.0f;
		m_fUnit_W = 0.0f;
		m_fUnit_H = 0.0f;
		m_dwCellNum = 0;
		ppCellAry = NULL;
		m_uiDim = 0;

		// �e���x���ł̋�Ԑ����Z�o
		int i;
		m_iPow[0] = 1;
		for (i = 1; i<CLINER4TREEMANAGER_MAXLEVEL + 1; i++)
			m_iPow[i] = m_iPow[i - 1] * 4;
	}
	template<class T>
	IKD::CLiner4TreeManager<T>::~CLiner4TreeManager() {
		DWORD i;
		for (i = 0; i<m_dwCellNum; i++) {
			if (ppCellAry[i] != NULL)
				delete ppCellAry[i];
		}
		delete[] ppCellAry;
	}
	template<class T>
	bool IKD::CLiner4TreeManager<T>::Init(unsigned int Level, double left, double top, double right, double bottom){
		// �ݒ�ō����x���ȏ�̋�Ԃ͍��Ȃ�
		if (Level >= CLINER4TREEMANAGER_MAXLEVEL)
			return false;

		// Level���x���i0��_�j�̔z��쐬
		m_dwCellNum = (m_iPow[Level + 1] - 1) / 3;	//��4^i(i<Level)�Ԃ�̔z����m�ہA�}�W�b�N�i���o�[�̂����ŕ�����Â炢���P�Ȃ铙�䐔��̘a
		ppCellAry = new CCell<T>*[m_dwCellNum];
		ZeroMemory(ppCellAry, sizeof(CCell<T>*)*m_dwCellNum);	//�[���N���A

		// �̈��o�^,
		m_fLeft = left;
		m_fTop = top;
		m_fW = right - left;
		m_fH = bottom - top;
		m_fUnit_W = m_fW / (1 << Level);
		m_fUnit_H = m_fH / (1 << Level);

		m_uiLevel = Level;

		return true;
	}
	template<class T>
	bool IKD::CLiner4TreeManager<T>::Regist(double left, double top, double right, double bottom, OBJECT_FOR_TREE<T> *pOFT)
	{
		// �I�u�W�F�N�g�̋��E�͈͂���o�^���[�g���ԍ����Z�o
		DWORD Elem = GetMortonNumber(left, top, right, bottom);
		if (Elem < m_dwCellNum) {
			// ��Ԃ������ꍇ�͐V�K�쐬
			if (!ppCellAry[Elem])
				CreateNewCell(Elem);
			return ppCellAry[Elem]->Push(pOFT);
		}
		return false;	// �o�^���s
	}
	template<class T>
	DWORD IKD::CLiner4TreeManager<T>::GetAllCollisionList(CollisionList<T>** colList){
		// ���X�g�i�z��j�͕K�����������܂�
		m_ColList.resetPos();

		// ���[�g��Ԃ̑��݂��`�F�b�N
		if (ppCellAry[0] == NULL)
			return 0;	// ��Ԃ����݂��Ă��Ȃ�

		// ���[�g��Ԃ�����
		std::list<T*> ColStac;
		GetCollisionList(0, ColStac);

		*colList = &m_ColList;

		return (DWORD)m_ColList.getSize();
	}
	template<class T>
	bool IKD::CLiner4TreeManager<T>::GetCollisionList(DWORD Elem, std::list<T*> &ColStac){
		std::list<T*>::iterator it;
		// �@ ��ԓ��̃I�u�W�F�N�g���m�̏Փ˃��X�g�쐬
		OBJECT_FOR_TREE<T>* pOFT1 = ppCellAry[Elem]->GetFirstObj();
		while (pOFT1 != 0)
		{
			OBJECT_FOR_TREE<T>* pOFT2 = pOFT1->m_pNext;
			while (pOFT2 != 0) {
				m_ColList.wright(pOFT1->m_pObject, pOFT2->m_pObject);
				pOFT2 = pOFT2->m_pNext;
			}
			// �A �Փ˃X�^�b�N�Ƃ̏Փ˃��X�g�쐬
			for (it = ColStac.begin(); it != ColStac.end(); it++) {
				m_ColList.wright(pOFT1->m_pObject, *it);
			}
			pOFT1 = pOFT1->m_pNext;
		}

		bool ChildFlag = false;
		// �B �q��ԂɈړ�
		DWORD ObjNum = 0;
		DWORD i, NextElem;
		for (i = 0; i<4; i++) {
			NextElem = Elem * 4 + 1 + i;
			if (NextElem<m_dwCellNum && ppCellAry[Elem * 4 + 1 + i]) {
				if (!ChildFlag) {
					// �C �o�^�I�u�W�F�N�g���X�^�b�N�ɒǉ�
					pOFT1 = ppCellAry[Elem]->GetFirstObj();
					while (pOFT1 != 0) {
						ColStac.push_back(pOFT1->m_pObject);
						ObjNum++;
						pOFT1 = pOFT1->m_pNext;
					}
				}
				ChildFlag = true;
				GetCollisionList(Elem * 4 + 1 + i, ColStac);	// �q��Ԃ�
			}
		}

		// �D �X�^�b�N����I�u�W�F�N�g���O��
		if (ChildFlag) {
			for (i = 0; i<ObjNum; i++)
				ColStac.pop_back();
		}

		return true;
	}
	template<class T>
	bool IKD::CLiner4TreeManager<T>::CreateNewCell(DWORD Elem){
		// �����̗v�f�ԍ�
		while (!ppCellAry[Elem])
		{
			// �w��̗v�f�ԍ��ɋ�Ԃ�V�K�쐬
			ppCellAry[Elem] = new CCell<T>;

			// �e��ԂɃW�����v
			Elem = (Elem - 1) >> 2;
			if (Elem >= m_dwCellNum) break;
		}
		return true;
	}

	template<class T>
	DWORD IKD::CLiner4TreeManager<T>::GetMortonNumber(double left, double top, double right, double bottom)
	{
		// �ŏ����x���ɂ�����e���ʒu���Z�o
		DWORD LT = GetPointElem(left, top);
		DWORD RB = GetPointElem(right, bottom);

		// ��Ԕԍ��̔r���I�_���a����
		// �������x�����Z�o
		DWORD Def = RB ^ LT;
		unsigned int HiLevel = 0;
		unsigned int i;
		for (i = 0; i<m_uiLevel; i++)
		{
			DWORD Check = (Def >> (i * 2)) & 0x3;
			if (Check != 0)
				HiLevel = i + 1;
		}
		DWORD SpaceNum = RB >> (HiLevel * 2);
		DWORD AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 3;
		SpaceNum += AddNum;

		if (SpaceNum > m_dwCellNum)
			return 0xffffffff;

		return SpaceNum;
	}
	DWORD IKD::CLiner4TreeManager<T>::BitSeparate32(DWORD n)
	{
		n = (n | (n << 8)) & 0x00ff00ff;
		n = (n | (n << 4)) & 0x0f0f0f0f;
		n = (n | (n << 2)) & 0x33333333;
		return (n | (n << 1)) & 0x55555555;
	}

	WORD IKD::CLiner4TreeManager<T>::Get2DMortonNumber(WORD x, WORD y)
	{
		return (WORD)(BitSeparate32(x) | (BitSeparate32(y) << 1));
	}

	DWORD IKD::CLiner4TreeManager<T>::GetPointElem(double pos_x, double pos_y)
	{
		return Get2DMortonNumber((WORD)((pos_x - m_fLeft) / m_fUnit_W), (WORD)((pos_y - m_fTop) / m_fUnit_H));
	}
	

}

