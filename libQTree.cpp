#include "libQTree.h"


template< class T>
void IKD::CCell<T>::ResetLink(OBJECT_FOR_TREE<T> *pOFT) {
	//再帰でオブジェクトリストを最後まで辿って、最後から順番にポインタNextを0で潰す
	if (pOFT->m_pNext != 0) {
		ResetLink(pOFT->m_pNext);
	}
	pOFT = 0;
}
template< class T>
bool IKD::CCell<T>::Push(OBJECT_FOR_TREE<T> *pOFT) {
	if (pOFT == 0) return false;	// 無効オブジェクトは登録しない
	if (pOFT->m_pCell == this) return false;	// 2重登録チェック、すでにこの空間に登録されているなら登録しない
	if (m_pLatest == 0) {
		m_pLatest = pOFT;	// 空間に新規登録
	}
	else
	{
		// 最新OFTオブジェクトを更新
		pOFT->m_pNext = m_pLatest;		//今追加するオブジェクトの次のオブジェクトとして今の最新オブジェクトを設定
		m_pLatest->m_pPre = pOFT;		//今の最新の前のオブジェクトとして今回追加するオブジェクトを設定
		m_pLatest = pOFT;				//最新オブジェクトは今追加したものだ
	}
	pOFT->m_pCell = this;	// 空間を登録
	return true;
}



template<class T>
bool IKD::CCell<T>::OnRemove(OBJECT_FOR_TREE<T> *pRemoveObj)
{
	if (m_pLatest == pRemoveObj) {
		// 次のオブジェクトに挿げ替え
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

		// 各レベルでの空間数を算出
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
		// 設定最高レベル以上の空間は作れない
		if (Level >= CLINER4TREEMANAGER_MAXLEVEL)
			return false;

		// Levelレベル（0基点）の配列作成
		m_dwCellNum = (m_iPow[Level + 1] - 1) / 3;	//Σ4^i(i<Level)個ぶんの配列を確保、マジックナンバーのせいで分かりづらいが単なる等比数列の和
		ppCellAry = new CCell<T>*[m_dwCellNum];
		ZeroMemory(ppCellAry, sizeof(CCell<T>*)*m_dwCellNum);	//ゼロクリア

		// 領域を登録,
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
		// オブジェクトの境界範囲から登録モートン番号を算出
		DWORD Elem = GetMortonNumber(left, top, right, bottom);
		if (Elem < m_dwCellNum) {
			// 空間が無い場合は新規作成
			if (!ppCellAry[Elem])
				CreateNewCell(Elem);
			return ppCellAry[Elem]->Push(pOFT);
		}
		return false;	// 登録失敗
	}
	template<class T>
	DWORD IKD::CLiner4TreeManager<T>::GetAllCollisionList(CollisionList<T>** colList){
		// リスト（配列）は必ず初期化します
		m_ColList.resetPos();

		// ルート空間の存在をチェック
		if (ppCellAry[0] == NULL)
			return 0;	// 空間が存在していない

		// ルート空間を処理
		std::list<T*> ColStac;
		GetCollisionList(0, ColStac);

		*colList = &m_ColList;

		return (DWORD)m_ColList.getSize();
	}
	template<class T>
	bool IKD::CLiner4TreeManager<T>::GetCollisionList(DWORD Elem, std::list<T*> &ColStac){
		std::list<T*>::iterator it;
		// ① 空間内のオブジェクト同士の衝突リスト作成
		OBJECT_FOR_TREE<T>* pOFT1 = ppCellAry[Elem]->GetFirstObj();
		while (pOFT1 != 0)
		{
			OBJECT_FOR_TREE<T>* pOFT2 = pOFT1->m_pNext;
			while (pOFT2 != 0) {
				m_ColList.wright(pOFT1->m_pObject, pOFT2->m_pObject);
				pOFT2 = pOFT2->m_pNext;
			}
			// ② 衝突スタックとの衝突リスト作成
			for (it = ColStac.begin(); it != ColStac.end(); it++) {
				m_ColList.wright(pOFT1->m_pObject, *it);
			}
			pOFT1 = pOFT1->m_pNext;
		}

		bool ChildFlag = false;
		// ③ 子空間に移動
		DWORD ObjNum = 0;
		DWORD i, NextElem;
		for (i = 0; i<4; i++) {
			NextElem = Elem * 4 + 1 + i;
			if (NextElem<m_dwCellNum && ppCellAry[Elem * 4 + 1 + i]) {
				if (!ChildFlag) {
					// ④ 登録オブジェクトをスタックに追加
					pOFT1 = ppCellAry[Elem]->GetFirstObj();
					while (pOFT1 != 0) {
						ColStac.push_back(pOFT1->m_pObject);
						ObjNum++;
						pOFT1 = pOFT1->m_pNext;
					}
				}
				ChildFlag = true;
				GetCollisionList(Elem * 4 + 1 + i, ColStac);	// 子空間へ
			}
		}

		// ⑤ スタックからオブジェクトを外す
		if (ChildFlag) {
			for (i = 0; i<ObjNum; i++)
				ColStac.pop_back();
		}

		return true;
	}
	template<class T>
	bool IKD::CLiner4TreeManager<T>::CreateNewCell(DWORD Elem){
		// 引数の要素番号
		while (!ppCellAry[Elem])
		{
			// 指定の要素番号に空間を新規作成
			ppCellAry[Elem] = new CCell<T>;

			// 親空間にジャンプ
			Elem = (Elem - 1) >> 2;
			if (Elem >= m_dwCellNum) break;
		}
		return true;
	}

	template<class T>
	DWORD IKD::CLiner4TreeManager<T>::GetMortonNumber(double left, double top, double right, double bottom)
	{
		// 最小レベルにおける各軸位置を算出
		DWORD LT = GetPointElem(left, top);
		DWORD RB = GetPointElem(right, bottom);

		// 空間番号の排他的論理和から
		// 所属レベルを算出
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

