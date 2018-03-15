#include "common.h"
#include "funcation.h"

int main()
{
//纠错码 小测
/*	unsigned char Mes[] = { 32 ,65, 205, 69 ,41, 220, 46 ,128, 236 };
	int N = sizeof(Mes) / sizeof(unsigned char);
	unsigned char result[17];
	GenerErrorCorCode(Mes, N, result, 17);
*/
//	wchar_t data1[] = { L"你好" };
//	char *data = (char *)data1;
//	char data[1024];
//	gets_s(data_input);

//	char data[1024] = "hello zengming 123666";

//	汉字转日文汉字方式，只有部分汉字可以。
//	int wcharlen = MultiByteToWideChar(936, 0, data_input, -1, NULL, 0);
//	wchar_t datawch[100];
//	MultiByteToWideChar(936, 0, data_input, -1, datawch, wcharlen);
//	Q = AllVersion[(1 - 1) * 4 + VersionLevel];
//	WideCharToMultiByte(932, 0, wch, -1,data, 1024, NULL, NULL);

	cout << "请输入文本或网址信息（按回车结束）：";
	QRVersion Q;
	char data[2048] = { "hello zengming 123666" };
	gets_s(data);

	DataAnalysis(data, Q); 
	unsigned char *DataCodeWords = new unsigned char[Q.DataCodeWordsNo];//记得释放
	DataEnCoding(Q,data, DataCodeWords);

	char *bitString = new char[(Q.DataCodeWordsNo + Q.ECCodeWordsNo*(Q.Block1No + Q.Block2No) )* 8 + 1];//记得释放
	StructMes(Q, DataCodeWords, bitString);

	char m[177][177] = { 0 };
	ModulePlacement(Q, m, bitString);

	char mask[177][177] = { 0 };
	int pmode;
	Masking(Q, m, mask, pmode);

	SetFormatVersion(Q, mask, pmode);

	ScreenOutput(Q, mask);
	
	delete DataCodeWords;//释放空间
	delete bitString;
	
	getchar();
	return 0;
}