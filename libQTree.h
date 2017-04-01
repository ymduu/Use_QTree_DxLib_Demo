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
	//marupeke_IKD氏のQTreeを(勉強のため)書き直した(ほとんど写経)ものなので、namespaceはIKDとする
	//命名規則はサンプルコードとまるぺけのページに準拠しますが、実装はcppの方に書きます

	template <class T>
	class CCell;

	//4分木に登録するオブジェクト
	template< class T>
	class OBJECT_FOR_TREE
	{
	public:
		CCell<T> *m_pCell;				// 登録空間
		T *m_pObject;					// 判定対象オブジェクト
		OBJECT_FOR_TREE<T>* m_pPre;		// 前のOBJECT_FOR_TREE構造体 、スマートポインタではないのでm_pPreに変更
		OBJECT_FOR_TREE<T>* m_pNext;	// 次のOBJECT_FOR_TREE構造体
		int m_id;
		OBJECT_FOR_TREE(int id = 0);

		virtual ~OBJECT_FOR_TREE() {}

		// 自らリストから外れる
		bool Remove();

	};
	//空間クラス、ここにOBJECT_FOR_TREEを登録する
	template <class T>
	class CCell
	{
	protected:
		OBJECT_FOR_TREE<T> *m_pLatest;	// 最新OFLへのスマートじゃないポインタ

	public:
		// コンストラクタ
		CCell() : m_pLatest(0)
		{
		}

		// デストラクタ
		virtual ~CCell()
		{
			if (m_pLatest != NULL)
				ResetLink(m_pLatest);
		}

		// リンクを全てリセットする
		void ResetLink(OBJECT_FOR_TREE<T> *pOFT);

		// OFTをプッシュ
		bool Push(OBJECT_FOR_TREE<T> *pOFT);

		OBJECT_FOR_TREE<T> *GetFirstObj() {
			return m_pLatest;
		}

		// 削除されるオブジェクトをチェック
		bool OnRemove(OBJECT_FOR_TREE<T> *pRemoveObj);
	};

#define COLLISIONLIST_REALLOCSIZE	100
	template < class T >
	class CollisionList {
	public:
		CollisionList() : root_(0), pos_(0), mallocSize_(0) {
			//はじめmallocsizeは0で0バイト確保する
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
				//もしmallocsize以上欲しいことになっていたらreallocする
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
		T**		root_;		// リストルートポインタ
		size_t	pos_;
		size_t	mallocSize_;
	};

	template <class T>
	class CLiner4TreeManager
	{
	protected:
		//最近はfloatを使う意味はないだの結局doubleに変換するから遅いだの散々言われているので全部doubleで扱う
		unsigned int m_uiDim;
		CCell<T> **ppCellAry;			// 線形空間ポインタ配列("線形の"空間ポインタ配列)
		unsigned int m_iPow[CLINER4TREEMANAGER_MAXLEVEL + 1];	// べき乗数値配列
		double m_fW;					// 領域のX軸幅
		double m_fH;					// 領域のY軸幅
		double m_fLeft;					// 領域の左側（X軸最小値）
		double m_fTop;					// 領域の上側（Y軸最小値）
		double m_fUnit_W;				// 最小レベル空間の幅単位
		double m_fUnit_H;				// 最小レベル空間の高単位
		DWORD m_dwCellNum;				// 空間の数
		unsigned int m_uiLevel;			// 最下位レベル
		CollisionList< T > m_ColList;	// 衝突リスト

	public:
		// コンストラクタ
		CLiner4TreeManager();

		// デストラクタ
		virtual ~CLiner4TreeManager();

		// 線形4分木配列を構築する
		bool Init(unsigned int Level, double left, double top, double right, double bottom);

		// オブジェクトを登録する
		bool Regist(double left, double top, double right, double bottom, OBJECT_FOR_TREE<T> *pOFT);

		// 衝突判定リストを作成する
		DWORD GetAllCollisionList(CollisionList<T>** colList);

	protected:

		// 空間内で衝突リストを作成する
		bool GetCollisionList(DWORD Elem, std::list<T*> &ColStac);


		// 空間を生成
		bool CreateNewCell(DWORD Elem);

		// 座標から空間番号を算出
		DWORD GetMortonNumber(double left, double top, double right, double bottom);


		// ビット分割関数
		DWORD BitSeparate32(DWORD n);

		// 2Dモートン空間番号算出関数
		WORD Get2DMortonNumber(WORD x, WORD y);

		// 座標→線形4分木要素番号変換関数
		DWORD GetPointElem(double pos_x, double pos_y);

	};


}
