#include "common.h"
#include "funcation.h"

/*  **************************************
	函数功能：将数据分块产生纠错码且按规则排列
	Q：版本信息
	data：数据码
	bitString：结果比特串指针
*/
void StructMes(const QRVersion &Q, unsigned char *data, char *bitString)
{
	int BlockNo = Q.Block1No + Q.Block2No;
	int *BlockAddr = new int[BlockNo];//记得释放
	unsigned char  *ErrorCode = new unsigned char[BlockNo*Q.ECCodeWordsNo];//记得释放
	
	int i, j, total;
	for (i = 0,j=0; i < Q.Block1No; i++)//块类型1
	{
		BlockAddr[i] = j;
		GenerErrorCorCode(data + j, Q.Block1DataCodeWordsNo, ErrorCode + i*Q.ECCodeWordsNo, Q.ECCodeWordsNo);
		j += Q.Block1DataCodeWordsNo;
	}
	for (; i < BlockNo; i++)//块类型2（如果有）
	{
		BlockAddr[i] = j;
		GenerErrorCorCode(data + j, Q.Block2DataCodeWordsNo, ErrorCode + i*Q.ECCodeWordsNo, Q.ECCodeWordsNo);
		j += Q.Block2DataCodeWordsNo;
	}

	int maxBlock = Q.Block1DataCodeWordsNo>Q.Block2DataCodeWordsNo ? Q.Block1DataCodeWordsNo : Q.Block2DataCodeWordsNo;
	for (i = 0, total = 0; i < maxBlock; i++)
		for (j = 0; j < BlockNo; j++)
			if (j < Q.Block1No&&i < Q.Block1DataCodeWordsNo)
				ToBit(data[BlockAddr[j] + i], bitString + 8 * total++, 8);
			else if (j >= Q.Block1No)
				ToBit(data[BlockAddr[j] + i], bitString + 8 * total++, 8);

	for (i = 0; i < Q.ECCodeWordsNo; i++)
		for (j = 0; j < BlockNo; j++)
			ToBit(ErrorCode[j*Q.ECCodeWordsNo + i], bitString + 8 * total++, 8);

	bitString[total * 8] = '\0';
	delete BlockAddr;//释放空间
	delete ErrorCode;//释放空间
}
