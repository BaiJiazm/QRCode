#include "common.h"
#include "funcation.h"

/*  **************************************
	�������ܣ������ݷֿ�����������Ұ���������
	Q���汾��Ϣ
	data��������
	bitString��������ش�ָ��
*/
void StructMes(const QRVersion &Q, unsigned char *data, char *bitString)
{
	int BlockNo = Q.Block1No + Q.Block2No;
	int *BlockAddr = new int[BlockNo];//�ǵ��ͷ�
	unsigned char  *ErrorCode = new unsigned char[BlockNo*Q.ECCodeWordsNo];//�ǵ��ͷ�
	
	int i, j, total;
	for (i = 0,j=0; i < Q.Block1No; i++)//������1
	{
		BlockAddr[i] = j;
		GenerErrorCorCode(data + j, Q.Block1DataCodeWordsNo, ErrorCode + i*Q.ECCodeWordsNo, Q.ECCodeWordsNo);
		j += Q.Block1DataCodeWordsNo;
	}
	for (; i < BlockNo; i++)//������2������У�
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
	delete BlockAddr;//�ͷſռ�
	delete ErrorCode;//�ͷſռ�
}
