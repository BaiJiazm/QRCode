#include "common.h"
#include "funcation.h"

typedef BOOL(WINAPI *PROCSETCONSOLEFONT)(HANDLE, DWORD);

void setcolor(HANDLE hout, const int bg_color, const int fg_color)
{
	SetConsoleTextAttribute(hout, bg_color * 16 + fg_color);
}
void setconsolefont(const HANDLE hout, const int font_no)
{
	HMODULE hKernel32 = GetModuleHandleA("kernel32");
	PROCSETCONSOLEFONT setConsoleFont = (PROCSETCONSOLEFONT)GetProcAddress(hKernel32, "SetConsoleFont");
	setConsoleFont(hout, font_no);
	return;
}
void setconsoleborder(const HANDLE hout, const int cols, const int lines)
{
	char cmd[80];

	system("cls");
	sprintf(cmd, "mode con cols=%d lines=%d", cols, lines);
	system(cmd);

	return;
}

const int NormalBlank = 0;
const int NormalWhite = 15;
const int reservedFg = 9;
const int ZeroFg = 15;
void ScreenOutput(const QRVersion &Q,char(*m)[177])
{
	const HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	const int size = Q.SideSize;
	if (Q.Version>6)
	    setconsolefont(hout, 0);
	else 
		setconsolefont(hout, 1);

	setconsoleborder(hout, 2 * (size + 4), size + 4);

	int i, j;
	for (i = -1; i <= size; i++)
	{
		for (j = -1; j <= size; j++)
		{
			if (i == -1 || i == size || j == -1 || j == size || m[i][j] == 2 || m[i][j] == 4)
				setcolor(hout, NormalWhite, NormalWhite);
			else if (m[i][j] == 1 || m[i][j] == 3)
				setcolor(hout, NormalBlank, NormalBlank);
			else if (m[i][j] == 0)
				setcolor(hout, ZeroFg, ZeroFg);
			else if (m[i][j] == -1)
				setcolor(hout, reservedFg, reservedFg);
			cout << "  ";
		}
		cout << endl;
	}
}