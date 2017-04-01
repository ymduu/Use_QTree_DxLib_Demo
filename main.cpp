#include "DxLib.h"
#include "libQTree.h"
#include <memory>
#include <vector>

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


// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(800, 600, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	Circle cs[500];
	IKD::OBJECT_FOR_TREE<Circle>* spOFTAry[500];
	int c = 0;
	for (int i = 1; i < 10; i++) {
		for (int j = 1; j < 15; j++) {
			cs[c].InitCircle(j * 50, i * 50, 15, GetColor(255, 0, 255), TRUE);
			cs[c].Draw();
			
			// OFTに登録
			IKD::OBJECT_FOR_TREE<Circle> *p = new IKD::OBJECT_FOR_TREE<Circle>(c);
			p->m_pObject = &cs[c];	// 登録
			spOFTAry[c] = p;
			c++;
		}
	}
	IKD::CLiner4TreeManager<Circle> LTree;
	if (!LTree.Init(8, 0.0, 0.0, 600.0, 800.0))
	{
		MessageBox(NULL, _T("線形4分木空間の初期化に失敗しました。"), NULL, NULL);
		return 0;
	}
	
	//描画先を裏画面に設定
	SetDrawScreen(DX_SCREEN_BACK);
	char key[256];
	IKD::CollisionList<Circle>* ColVect;	
	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && GetHitKeyStateAll(key) == 0) {
		//再登録
		static bool change = true;
		for (int cn = 0; cn < c; cn++)
		{
			int i = cn;
			if (change == true) {
				i = c - cn - 1;
			}
			Circle *pTmp = spOFTAry[i]->m_pObject;
			spOFTAry[i]->Remove();		// 一度リストから外れる
										// 再登録
			LTree.Regist(pTmp->x - pTmp->r, pTmp->y - pTmp->r, pTmp->x + pTmp->r, pTmp->y + pTmp->r, spOFTAry[i]);
		}
		change = !change;
		int ColNum = LTree.GetAllCollisionList(&ColVect);
		// 衝突判定
		DWORD cir;
		ColNum /= 2;	// 2で割るのはペアになっているので
		Circle** pRoot = ColVect->getRootPtr();
		for (cir = 0; cir<ColNum; cir++) {
			float r2 = (pRoot[cir * 2]->r + pRoot[cir * 2 + 1]->r)*(pRoot[cir * 2]->r + pRoot[cir * 2 + 1]->r);
			float x = (pRoot[cir * 2]->x - pRoot[cir * 2 + 1]->x);
			float y = (pRoot[cir * 2]->y - pRoot[cir * 2 + 1]->y);
			if (r2 >= x*x + y*y)
			{
				// ぶつかった物同士のテクスチャをオレンジに変更
				pRoot[cir * 2]->color = GetColor(255, 165, 0);
				pRoot[cir * 2 + 1]->color = GetColor(255, 165, 0);

			}
		}


		for (int i = 0; i < c; i++) {
			cs[i].Draw();
		}
		
		int move = 4;
		if (key[KEY_INPUT_LEFT] == 1) {
			cs[0].x -= move;
			cs[0].color = GetColor(255, 0, 0);
		}
		if (key[KEY_INPUT_RIGHT] == 1) {
			cs[0].x += move;
		}
		if (key[KEY_INPUT_UP] == 1) {
			cs[0].y -= move;
		}
		if (key[KEY_INPUT_DOWN] == 1) {
			cs[0].y += move;
		}

		if (key[KEY_INPUT_ESCAPE] == 1) {
			break;
		}

	}


	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}