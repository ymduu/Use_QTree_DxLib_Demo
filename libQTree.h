#pragma once

#include <set>
#include <vector>
#include <list>
#include <map>
#include <memory>

namespace IKD {
	//marupeke_IKD氏のQTreeを(勉強のため)書き直したものなので、namespaceはIKDとする
	//命名規則はサンプルコードとまるぺけのページに準拠しますが、実装はcppの方に書きます
	template <class T>
	class CCell;	//空間クラス、ポインタを下で使うのに宣言だけ必要なため

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

		virtual ~OBJECT_FOR_TREE(){}

		// 自らリストから外れる
		bool Remove();

	};


}
