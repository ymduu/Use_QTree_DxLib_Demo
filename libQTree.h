#pragma once

#include <set>
#include <vector>
#include <list>
#include <map>
#include <memory>

namespace IKD {
	//marupeke_IKD����QTree��(�׋��̂���)�������������̂Ȃ̂ŁAnamespace��IKD�Ƃ���
	//�����K���̓T���v���R�[�h�Ƃ܂�؂��̃y�[�W�ɏ������܂����A������cpp�̕��ɏ����܂�
	template <class T>
	class CCell;	//��ԃN���X�A�|�C���^�����Ŏg���̂ɐ錾�����K�v�Ȃ���

	//4���؂ɓo�^����I�u�W�F�N�g
	template< class T>
	class OBJECT_FOR_TREE
	{
	public:
		CCell<T> *m_pCell;				// �o�^���
		T *m_pObject;					// ����ΏۃI�u�W�F�N�g
		OBJECT_FOR_TREE<T>* m_pPre;		// �O��OBJECT_FOR_TREE�\���� �A�X�}�[�g�|�C���^�ł͂Ȃ��̂�m_pPre�ɕύX
		OBJECT_FOR_TREE<T>* m_pNext;	// ����OBJECT_FOR_TREE�\����
		int m_id;
		OBJECT_FOR_TREE(int id = 0);

		virtual ~OBJECT_FOR_TREE(){}

		// ���烊�X�g����O���
		bool Remove();

	};


}
