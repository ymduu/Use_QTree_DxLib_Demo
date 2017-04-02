#include "DxLib.h"
#include "libQTree.h"
#include <memory>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#define DIVLEVEL 3.0
#define CIRARRSIZE 5000

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

void QTreeDemo() {
	Circle cs[CIRARRSIZE];
	//OFT��shared_ptr�Ŏ���
	std::shared_ptr<IKD::OBJECT_FOR_TREE<Circle>> spOFTAry[CIRARRSIZE];

	int circleNum = 0;
	srand((unsigned)time(NULL));
	for (int i = 1; i < 10; i++) {
		for (int j = 1; j < 15; j++) {
			cs[circleNum].InitCircle(j * 50 + rand() % 50, i * 50 + rand() % 50, 15, GetColor(255, 0, 255), circleNum);
			cs[circleNum].Draw();

			// OFT�ɓo�^
			spOFTAry[circleNum] = std::make_shared<IKD::OBJECT_FOR_TREE<Circle>>(circleNum);	//�V����OFT��shared_ptr�ō쐬
			spOFTAry[circleNum]->m_pObject = &cs[circleNum];	// �Ή�����~��o�^
			circleNum++;
		}
	}
	IKD::CLiner4TreeManager<Circle> LTree;
	if (!LTree.Init(DIVLEVEL, 0.0, 0.0, 800.0, 600.0))
	{
		MessageBox(NULL, _T("���`4���؋�Ԃ̏������Ɏ��s���܂����B"), NULL, NULL);
		return;
	}

	//�`���𗠉�ʂɐݒ�
	SetDrawScreen(DX_SCREEN_BACK);
	char key[256];
	IKD::CollisionList<Circle>* ColVect;
	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && GetHitKeyStateAll(key) == 0) {
		clsDx();
		//�ēo�^(�ړ��𔽉f�����邽��)

		for (int cn = 0; cn < circleNum; cn++){
			int i = cn;

			Circle *pTmp = spOFTAry[i]->m_pObject;
			spOFTAry[i]->Remove();		// ��x���X�g����O���
			pTmp->color = GetColor(255, 0, 255);
			// �ēo�^
			LTree.Regist(pTmp->x - pTmp->r, pTmp->y - pTmp->r, pTmp->x + pTmp->r, pTmp->y + pTmp->r, spOFTAry[i].get());
		}

		int ColNum = LTree.GetAllCollisionList(&ColVect);
		printfDx("possibleHitNum=%d", ColNum / 2);
		// �Փ˔���
		DWORD cir;
		ColNum /= 2;	// 2�Ŋ���̂̓y�A�ɂȂ��Ă���̂�
		Circle** pRoot = ColVect->getRootPtr();
		for (cir = 0; cir<ColNum; cir++) {
			float r2 = (pRoot[cir * 2]->r + pRoot[cir * 2 + 1]->r)*(pRoot[cir * 2]->r + pRoot[cir * 2 + 1]->r);
			float x = (pRoot[cir * 2]->x - pRoot[cir * 2 + 1]->x);
			float y = (pRoot[cir * 2]->y - pRoot[cir * 2 + 1]->y);
			if (r2 >= x*x + y*y)
			{
				// �Ԃ����������m�̐F���I�����W�ɕύX
				pRoot[cir * 2]->color = GetColor(255, 165, 0);
				pRoot[cir * 2 + 1]->color = GetColor(255, 165, 0);

			}
		}


		for (int i = 0; i < circleNum; i++) {
			cs[i].Draw();
		}
		//���Ƃ͓K���ɉ~�𓮂���+�����O���b�h�\���f��
		double xdiv, ydiv;
		xdiv = 800 / pow(2, DIVLEVEL);
		ydiv = 600 / pow(2, DIVLEVEL);

		for (int i = 0; i < pow(2, DIVLEVEL); i++) {
			DrawLine(xdiv*i, 0, xdiv*i, 600, GetColor(255, 255, 255), 1);
		}
		for (int i = 0; i < pow(2, DIVLEVEL); i++) {
			DrawLine(0, ydiv*i, 800, ydiv*i, GetColor(255, 255, 255), 1);
		}

		int move = 4;
		if (key[KEY_INPUT_LEFT] == 1) {
			cs[0].x -= move;
			//cs[0].color = GetColor(255, 0, 0);
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
}


// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(800, 600, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}
	QTreeDemo();

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}