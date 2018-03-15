#include "common.h"
#include "funcation.h"

void E1calcu(char &ch,int &penalty,int clear=0)
{/* 评价方式1的计算函数*/
	static int white, black;
	if (clear)
		white = black = 0;
	else if (ch)
	{
		white = 0;
		black++;
		if (black == 5)
			penalty += 3;
		else if (black > 5)
			penalty++;
	}
	else
	{
		white++;
		black = 0;
		if (white == 5)
			penalty += 3;
		else if (white > 5)
			penalty++;
	}
}
void OppositeBit(char &ch)
{/* 3代表深色 4代表浅色 */
	if (ch == 3)
		ch = 0;
	else if (ch == 4)
		ch = 1;
}

int Evaluate1(const QRVersion&Q, char(*m)[177])
{
	int penalty = 0;
	int i, j;
	for (i = 0; i < Q.SideSize; i++)
		for (j = 0; j < Q.SideSize; j++)
			E1calcu(m[i][j], penalty);

	E1calcu(m[0][0], penalty, 1);//清零数据

	for (i = 0; i < Q.SideSize; i++)
		for (j = 0; j < Q.SideSize; j++)
			E1calcu(m[j][i], penalty);
	return penalty;
}
int Evaluate2(const QRVersion&Q, char(*m)[177])
{
	int penalty = 0;
	int i, j;
	for (i = 0; i < Q.SideSize - 1; i++)
		for (j = 0; j < Q.SideSize - 1; j++)
			if (m[i][j] == m[i][j + 1] && m[i][j] == m[i + 1][j] && m[i][j] == m[i + 1][j + 1])
				penalty += 3;
	return penalty;
}
int Evaluate3(const QRVersion&Q, char(*m)[177])
{
	char find[] = { 1,0,1,1,1,0,1 };
	const int nfind = sizeof(find) / sizeof(char);
	int penalty = 0;
	int i, j, k;
	for (i = 0; i < Q.SideSize; i++)
		for (j = 0; j < Q.SideSize - nfind; j++)
		{
			for (k = 0; k < nfind; k++)
				if (find[k] != m[i][j + k])
					break;
			if (k == nfind)
				penalty += 40;

			for (k = 0; k < nfind; k++)
				if (find[k] != m[j + k][i])
					break;
			if (k == nfind)
				penalty += 40;
		}
	return penalty;
}
int Evaluate4(const QRVersion&Q, char(*m)[177])
{
	int penalty = 0;
	int black = 0;
	int i, j;
	for (i = 0; i < Q.SideSize; i++)
		for (j = 0; j < Q.SideSize; j++)
			black += m[i][j];
	black = int(((double)black / (Q.SideSize*Q.SideSize)) * 100);
	int low = black / 5 * 5;
	int up = (black + 4) % 5;
	low = abs(low - 50)/5;
	up = abs(up - 50)/5;
	penalty = 10 * (low > up ? up : low);
	return penalty;
}

void MaskingMode(const QRVersion &Q, const int Mode, char(*m)[177], char(*mask)[177])
{/* 进行八种掩模 */
	int row, column;
	for (row = 0; row < Q.SideSize; row++)
		for (column = 0; column < Q.SideSize; column++)
		{
			if (m[row][column] == -1 || m[row][column] == 2)
				mask[row][column] = 0;
			else
				mask[row][column] = m[row][column];
			if (mask[row][column] == 3 || mask[row][column] == 4)
			{
				if (Mode == 0 && (row + column) % 2 == 0)
					OppositeBit(mask[row][column]);
				else if (Mode == 1 && row % 2 == 0)
					OppositeBit(mask[row][column]);
				else if (Mode == 2 && column % 3 == 0)
					OppositeBit(mask[row][column]);
				else if (Mode == 3 && (row + column) % 3 == 0)
					OppositeBit(mask[row][column]);
				else if (Mode == 4 && (row / 2 + column / 3) % 2 == 0)
					OppositeBit(mask[row][column]);
				else if (Mode == 5 && (row * column) % 2 + (row * column) % 3 == 0)
					OppositeBit(mask[row][column]);
				else if (Mode == 6 && ((row * column) % 2 + (row * column) % 3) % 2 == 0)
					OppositeBit(mask[row][column]);
				else if (Mode == 7 && ((row + column) % 2 + (row * column) % 3) % 2 == 0)
					OppositeBit(mask[row][column]);
				mask[row][column] %= 2;
			}
		}
}
int EvaluateMask(const QRVersion&Q, char(*mask)[177])
{/* 评价掩模方式 */
	int penalty=0;
	int(*const p[])(const QRVersion&, char(*)[177]) = { Evaluate1 ,Evaluate2, Evaluate3, Evaluate4 };
	int i;
	for (i = 0; i < 4; i++)
		penalty += p[i](Q, mask);
	return penalty;
}

/* *********************************
	函数功能：进行掩模
	Q：版本信息	m：原矩阵	mask：掩模后的矩阵	pmode：以引用返回掩模方式
*/
void Masking(const QRVersion &Q, char(*m)[177], char(*mask)[177],int &pmode)
{
	int penalty = 0;
	pmode=0;
	int i, tem;
	for (i = 0; i < 8; i++)
	{
		MaskingMode(Q, i, m, mask);
		tem = EvaluateMask(Q, mask);
		if (i == 0)
			penalty = tem;
		if (penalty >= tem)
			pmode = i;
	}
	MaskingMode(Q, pmode, m, mask);
}