#pragma once

#include <set>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <windows.h>
#include "DxLib.h"

#define CLINER4TREEMANAGER_MAXLEVEL (9)


class Circle {
public:
	double x, y, r;
	int color, id;
	Circle(double a_x, double a_y, double a_r, int a_color, int a_id) {
		x = a_x;
		y = a_y;
		r = a_r;
		id = a_id;
		color = a_color;
		//color = GetColor(255, 0, 0);
	}
	Circle() {}
	void Draw() {
		DrawCircle(x, y, r, color, TRUE);
	}
	void InitCircle(double a_x, double a_y, double a_r, int a_color, int a_id) {
		x = a_x;
		y = a_y;
		r = a_r;
		id = a_id;
		color = a_color;
	}
};

namespace IKD {
	//marupeke_IKD����QTree��(�׋��̂���)����������(�قƂ�ǎʌo)���̂Ȃ̂ŁAnamespace��IKD�Ƃ���
	//�����K���̓T���v���R�[�h�Ƃ܂�؂��̃y�[�W�ɏ������܂����A������cpp�̕��ɏ����܂�

	template <class T>
	class CCell;

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

		virtual ~OBJECT_FOR_TREE() {}

		// ���烊�X�g����O���
		bool Remove();

	};
	//��ԃN���X�A������OBJECT_FOR_TREE��o�^����
	template <class T>
	class CCell
	{
	protected:
		OBJECT_FOR_TREE<T> *m_pLatest;	// �ŐVOFL�ւ̃X�}�[�g����Ȃ��|�C���^

	public:
		// �R���X�g���N�^
		CCell() : m_pLatest(0)
		{
		}

		// �f�X�g���N�^
		virtual ~CCell()
		{
			if (m_pLatest != NULL)
				ResetLink(m_pLatest);
		}

		// �����N��S�ă��Z�b�g����
		void ResetLink(OBJECT_FOR_TREE<T> *pOFT);

		// OFT���v�b�V��
		bool Push(OBJECT_FOR_TREE<T> *pOFT);

		OBJECT_FOR_TREE<T> *GetFirstObj() {
			return m_pLatest;
		}

		// �폜�����I�u�W�F�N�g���`�F�b�N
		bool OnRemove(OBJECT_FOR_TREE<T> *pRemoveObj);
	};

#define COLLISIONLIST_REALLOCSIZE	100
	template < class T >
	class CollisionList {
	public:
		CollisionList() : root_(0), pos_(0), mallocSize_(0) {
			//�͂���mallocsize��0��0�o�C�g�m�ۂ���
			root_ = (T**)malloc(0);
		}
		~CollisionList() {
			reflesh();
		}

		size_t getSize() {
			return pos_;
		}

		T** getRootPtr() {
			return root_;
		}

		void resetPos() {
			pos_ = 0;
		}

		void wright(T* obj1, T*obj2) {
			if (pos_ >= mallocSize_) {
				//����mallocsize�ȏ�~�������ƂɂȂ��Ă�����realloc����
				root_ = (T**)realloc(root_, sizeof(T*) * (mallocSize_ + COLLISIONLIST_REALLOCSIZE));
				mallocSize_ += COLLISIONLIST_REALLOCSIZE;
			}
			root_[pos_++] = obj1;
			root_[pos_++] = obj2;
		}

		void reflesh() {
			if (root_) {
				free(root_);
			}
		}

	private:
		T**		root_;		// ���X�g���[�g�|�C���^
		size_t	pos_;
		size_t	mallocSize_;
	};

	template <class T>
	class CLiner4TreeManager
	{
	protected:
		//�ŋ߂�float���g���Ӗ��͂Ȃ����̌���double�ɕϊ����邩��x�����̎U�X�����Ă���̂őS��double�ň���
		unsigned int m_uiDim;
		CCell<T> **ppCellAry;			// ���`��ԃ|�C���^�z��("���`��"��ԃ|�C���^�z��)
		unsigned int m_iPow[CLINER4TREEMANAGER_MAXLEVEL + 1];	// �ׂ��搔�l�z��
		double m_fW;					// �̈��X����
		double m_fH;					// �̈��Y����
		double m_fLeft;					// �̈�̍����iX���ŏ��l�j
		double m_fTop;					// �̈�̏㑤�iY���ŏ��l�j
		double m_fUnit_W;				// �ŏ����x����Ԃ̕��P��
		double m_fUnit_H;				// �ŏ����x����Ԃ̍��P��
		DWORD m_dwCellNum;				// ��Ԃ̐�
		unsigned int m_uiLevel;			// �ŉ��ʃ��x��
		CollisionList< T > m_ColList;	// �Փ˃��X�g

	public:
		// �R���X�g���N�^
		CLiner4TreeManager();

		// �f�X�g���N�^
		virtual ~CLiner4TreeManager();

		// ���`4���ؔz����\�z����
		bool Init(unsigned int Level, double left, double top, double right, double bottom);

		// �I�u�W�F�N�g��o�^����
		bool Regist(double left, double top, double right, double bottom, OBJECT_FOR_TREE<T> *pOFT);

		// �Փ˔��胊�X�g���쐬����
		DWORD GetAllCollisionList(CollisionList<T>** colList);

	protected:

		// ��ԓ��ŏՓ˃��X�g���쐬����
		bool GetCollisionList(DWORD Elem, std::list<T*> &ColStac);


		// ��Ԃ𐶐�
		bool CreateNewCell(DWORD Elem);

		// ���W�����Ԕԍ����Z�o
		DWORD GetMortonNumber(double left, double top, double right, double bottom);


		// �r�b�g�����֐�
		DWORD BitSeparate32(DWORD n);

		// 2D���[�g����Ԕԍ��Z�o�֐�
		WORD Get2DMortonNumber(WORD x, WORD y);

		// ���W�����`4���ؗv�f�ԍ��ϊ��֐�
		DWORD GetPointElem(double pos_x, double pos_y);

	};


}
