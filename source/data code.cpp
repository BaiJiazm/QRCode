#include "common.h"
#include "funcation.h"

/* 4种编码模式 */
const enum { Numeric, Alphanumeric, Byte, KanJi};

/* 相应编码Mode Indicator */
const char *ModeIndi[] = { "0001","0010","0100","1000" };

/* 相应版本编码模式计数bit数 */
const int CountIndiBit[][4] = {{10,9,8,8},{12,11,16,10},{14,13,16,12}};

/* 获得相应版本编码模式计数bit数 */
int GetCountIndiBit(int Version,int Mode)
{
	if (Version >= 1 && Version <= 9)
		Version = 0;
	else if (Version <= 26)
		Version = 1;
	else if (Version <= 40)
		Version = 2;

	return CountIndiBit[Version][Mode];
}

/* 将整数转为指定位数的‘0’‘1’位串 */
void ToBit(int I, char *bitChar,int NBit)
{
	int i;
	for (i = 0; i < NBit; i++)
		if (I & 1 << i)
			bitChar[NBit - i - 1] = '1';
		else
			bitChar[NBit - i - 1] = '0';
}

void BitToChar(unsigned char &result, char *BitChar)
{
	int i;
	for (i = 0; i < 8; i++)
		if (BitChar[i] == '1')
			result |= 0x80 >> i;
		else
			result &= ~(0x80 >> i);
}
void ByteMode(char *&data, char*& bitChar,int CountBit)
{
	int CByte;
	int i;
	for (CByte = 0; (unsigned char)data[CByte] > 0&&(unsigned char )data[CByte] < 0x7f; CByte++)
		;
	
	if (CByte)
	{
		strcpy(bitChar, ModeIndi[Byte]);
		ToBit(CByte, bitChar + 4, CountBit);

		bitChar += (4 + CountBit);
		for (i = 0; i < CByte; i++, bitChar += 8)
			ToBit(data[i], bitChar, 8);
	
		data += CByte;
		*bitChar = '\0';
	}
}

void KanJiMode(char *&data, char *&bitChar, int CountBit)
{
	int CKanJi;
	int i;
	unsigned char c1, c2;
	for (CKanJi = 0; data[CKanJi * 2] < 0; CKanJi++)
		;

	if (CKanJi)
	{
		strcpy(bitChar, ModeIndi[KanJi]);
		ToBit(CKanJi, bitChar + 4, CountBit);

		bitChar += (4 + CountBit);
		for (i = 0; i < CKanJi; i++, bitChar += 13)
		{
			c1 = data[i * 2];
			c2 = data[i * 2 + 1];
			if (c1 < 0xE0)
				ToBit((c1 - 0x81) * 0xC0 + (c2 - 0x40), bitChar, 13);
			else
				ToBit((c1 - 0xC1) * 0xC0 + (c2 - 0x40), bitChar, 13);
		}

		data += (CKanJi * 2);
		*bitChar = '\0';
	}
}

void ChineseMode(char *&data, char *&bitChar, int CountBit)
{
	int CChinese;
	int i;
	unsigned char c1, c2;
	for (CChinese = 0; data[CChinese * 2] < 0; CChinese++)
		;

	if (CChinese)
	{
		strcpy(bitChar, "11010001");
		ToBit(CChinese, bitChar +8, CountBit);

		bitChar += (8 + CountBit);
		for (i = 0; i < CChinese; i++, bitChar += 13)
		{
			c1 = data[i * 2];
			c2 = data[i * 2 + 1];
			if (c1 < 0xB0)
				ToBit((c1 - 0xA1) * 0x60 + (c2 - 0xA1), bitChar, 13);
			else 
				ToBit((c1 - 0xA6) * 0x60 + (c2 - 0xA1), bitChar, 13);
		}

		data += (CChinese * 2);
		*bitChar = '\0';
	}
}

void DataAnalysis(const char *data, QRVersion &Q)
{
	int CByteMode = 0;
	int CKanji = 0;
	int TotalDataBit = 0;

	int i;
	for (i = 0; data[i];)
		if (data[i] >0&&data[i] < 0x7f)
		{
			TotalDataBit += 8;
			if (i == 0)
				CByteMode++;
			else if (data[i-1]<0)
				CByteMode++;
			i++;
		}
		else
		{
			TotalDataBit += 13;
			if (i == 0)
				CKanji++;
			else if (data[i - 1] >0&&data[i - 1] < 0x7f)
				CKanji++;
			i += 2;
		}
	TotalDataBit += (CByteMode * 4 + CKanji * 8);

	for (i = 1; i <= 40; i++)
		if (TotalDataBit + CByteMode*GetCountIndiBit(i, Byte) + CKanji*GetCountIndiBit(i, KanJi) <= 8 * AllVersion[(i-1 )* 4 +VersionLevel].DataCodeWordsNo)
			break;

	Q = AllVersion[(i - 1) * 4 + VersionLevel];
}

void DataEnCoding(const QRVersion &Q,char *data, unsigned char *result)
{
	int TotalBit = Q.DataCodeWordsNo * 8;
	char *DataBit = new char[TotalBit+1+4];//记得释放

	char *pData = data;
	char *pDataBit = DataBit;

	for (; *pData != '\0';)
	{
		ByteMode(pData, pDataBit, GetCountIndiBit(Q.Version, Byte));
		ChineseMode(pData, pDataBit, GetCountIndiBit(Q.Version, KanJi));
	}
	
	strcpy(pDataBit, "0000");
	DataBit[TotalBit] = '\0';

	int CurDataBit = strlen(DataBit);
	int RemainDataBit = TotalBit - CurDataBit;
	int Set0Bit = RemainDataBit % 8;

	int i;
	for (i = 0; i < Set0Bit; i++)
		strcat(pDataBit, "0");

	RemainDataBit -= Set0Bit;
	for (i = 0; i < RemainDataBit; i += 8)
		if (i / 8 % 2 == 0)
			strcat(pDataBit, "11101100");
		else
			strcat(pDataBit, "00010001");

	for (i = 0; i < Q.DataCodeWordsNo; i++)
		BitToChar(result[i], DataBit + i * 8);
}
