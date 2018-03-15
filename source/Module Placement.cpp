#include "common.h"
#include "funcation.h"

/* 在二维码矩阵中指定位置填入矩阵 */
void PlaceMatrix(char(*m)[177], char *ori, const int oriCol, const int oriRow,const int left,const int top)
{
	int i, j;
	for (i = 0; i < oriRow; i++)
		for (j = 0; j < oriCol; j++)
			m[top + i][left + j] = *(ori + i*oriCol + j);
}

void SetFinder(const QRVersion &Q, char(*m)[177])
{
	/* 填充finder pattern 1代表深色，2代表浅色 */
	char finder[7][7] ={
	{ 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 2, 2, 2, 2, 2, 1 },
	{ 1, 2, 1, 1, 1, 2, 1 },
	{ 1, 2, 1, 1, 1, 2, 1 },
	{ 1, 2, 1, 1, 1, 2, 1 },
	{ 1, 2, 2, 2, 2, 2, 1 },
	{ 1, 1, 1, 1, 1, 1, 1 }};
	const int left1 = 0,				top1 = 0;
	const int left2 = Q.SideSize - 7,	top2 = 0;
	const int left3 = 0,				top3 = Q.SideSize - 7;
	PlaceMatrix(m, *finder, 7, 7, left1, top1);
	PlaceMatrix(m, *finder, 7, 7, left2, top2);
	PlaceMatrix(m, *finder, 7, 7, left3, top3);
}

void SetSeperator(const QRVersion &Q, char(*m)[177])
{
	/* 填充Seperator 位置 2代表浅色 */
	int i;
	for (i = 0; i < 8; i++)
	{
		m[7][i] = m[i][7] = 2;
		m[7][Q.SideSize - 8 + i] = m[i][Q.SideSize - 8] = 2;
		m[Q.SideSize - 8][i] = m[Q.SideSize - 8 + i][7] = 2;
	}
}

void SetAlignMent(const QRVersion &Q, char(*m)[177])
{
	/* 填充AlignMent 1代表深色，2代表浅色 */
	char align[5][5] = {
	{ 1, 1, 1, 1, 1 },
	{ 1, 2, 2, 2, 1 },
	{ 1, 2, 1, 2, 1 },
	{ 1, 2, 2, 2, 1 },
	{ 1, 1, 1, 1, 1 } };

	const int row1 = 7, row2 = Q.SideSize - 8;
	const int col1 = 7, col2 = Q.SideSize - 8;
	const int ncoord = AL[Q.Version - 1].ncoord;
	int i, j;
	int left, top;
	for (i = 0; i < ncoord; i++)
		for (j = 0; j < ncoord; j++)
		{
			left = AL[Q.Version - 1].coord[i] - 2;
			top = AL[Q.Version - 1].coord[j] - 2;
			if (i == 0 && j == 0 || i == 0 && j == ncoord - 1 || i == ncoord - 1 && j == 0)
				continue;
			PlaceMatrix(m, *align, 5, 5, left, top);
		}
}

void SetTiming(const QRVersion &Q, char(*m)[177])
{
	/* 填充Timing pattern 1代表深色，2代表浅色 */
	const int pos1 = 6, pos2 = Q.SideSize - 8;
	int i;
	for (i = pos1; i < pos2; i++)
		if (i % 2 == 0)
			m[i][pos1] = m[pos1][i] = 1;
		else
			m[i][pos1] = m[pos1][i] = 2;
}

void SetDark(const QRVersion &Q, char(*m)[177])
{
	/* 填充Dark pattern 1代表深色 */
	m[Q.Version * 4 + 9][8] = 1;
}

void FormatReserved(const QRVersion&Q, char(*m)[177])
{
	/* 保留Format区域，用-1填充区分 */
	const int pos1 = 8, pos2 = Q.SideSize - 8;
	int  j;
	for (j = 0; j <= pos1; j++)
		if (j != 6)
			m[j][pos1] = m[pos1][j] = -1;
	m[pos1][pos2] = -1;
	for (j = pos2 + 1; j < Q.SideSize; j++)
		m[pos1][j] = m[j][pos1] = -1;
}

void VersionReserved(const QRVersion&Q, char(*m)[177])
{
	/* 保留Version区域，用-1填充区分 */
	if (Q.Version < 7)
		return;
	const int pos = Q.SideSize - 11;
	int i, j;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 6; j++)
			m[pos + i][j] = m[j][pos + i] = -1;
}

void ConvertBitInfo(char &ori, char &des)
{/* 将bit字符串转化，4代表浅色，3代表深色 */
	if (ori == '0')
		des = 4;
	else if (ori == '1')
		des = 3;
}

/*	************************************
	函数功能：布置模块
	Q：版本信息
	m：二维码矩阵首行指针
	bitString：填充的数据bit字符串
*/
void ModulePlacement(const QRVersion&Q, char(*m)[177],char *bitString)
{
	SetFinder(Q, m);
	SetSeperator(Q, m);
	SetAlignMent(Q, m);
	SetTiming(Q, m);
	SetDark(Q, m);
	FormatReserved(Q, m);
	VersionReserved(Q, m);
	
	int maxside = (Q.Version - 1) * 4 + 21;
	int ColBlock = (Q.Version - 1) * 2 + 10;
	int RightPos;
	char *pbitString = bitString;
	int i, j;
	for (i = ColBlock - 1; i >= 0; i--)//填充数据区域
	{
		RightPos = i * 2 + 1;
		if (i > 2)
			RightPos++;
		if (i % 2)
			for (j = maxside - 1; j >= 0; j--)
			{
				if (m[j][RightPos] == 0)
					ConvertBitInfo(*pbitString++, m[j][RightPos]);
				if (m[j][RightPos - 1] == 0)
					ConvertBitInfo(*pbitString++, m[j][RightPos - 1]);
			}
		else
			for (j = 0; j < maxside; j++)
			{
				if (m[j][RightPos] == 0)
					ConvertBitInfo(*pbitString++, m[j][RightPos]);
				if (m[j][RightPos - 1] == 0)
					ConvertBitInfo(*pbitString++, m[j][RightPos - 1]);
			}
	}
}