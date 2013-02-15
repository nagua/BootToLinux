// GetUEFIVar.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <Windows.h>
#include <WinBase.h>


int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE handle = NULL;
	TOKEN_PRIVILEGES tp;
	DWORD result;
	TCHAR varname[] = _T("BootNext");
	TCHAR VendorGuiD[] = _T("{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}");
	//WORD setval[] = {0x4, 0x1A, 0x5, 0x3, 0x6, 0x7, 0xB, 0xD, 0xC, 0xF, 0x2, 0x10, 0x11, 0x12, 0x19, 0x13, 0x0, 0xE, 0x1};
	WORD setval = 0x1A;
	WORD getval;
	

	ZeroMemory(&tp, sizeof(tp));

	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &handle))
		printf("Failed to open process\n");
	if(!LookupPrivilegeValue(NULL, SE_SYSTEM_ENVIRONMENT_NAME, &tp.Privileges[0].Luid))
		printf("Failed to locate privileges\n");

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if(!AdjustTokenPrivileges(handle, FALSE, &tp, 0, NULL, 0))
		printf("Failed to adjust privileges");

	if(!SetFirmwareEnvironmentVariable(varname, VendorGuiD, &setval, sizeof(setval)))
		printf("Could not set value!");


	if(!GetFirmwareEnvironmentVariable(varname, VendorGuiD, &getval, sizeof(getval)))
		printf("No data get\n");

	//for(int i = 0; i < 20; i++)
		printf("BootNext: %X\n", getval);
	printf("\n");

	system("pause");
	return 0;
}

