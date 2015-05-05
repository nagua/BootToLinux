// BootToLinux.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"

TCHAR* getProgName(TCHAR*);
bool setToken(TCHAR*);
bool setBootNext(WORD);

int _tmain(int argc, _TCHAR* argv[])
{
	//WORD setval[] = {0x4, 0x1A, 0x5, 0x3, 0x6, 0x7, 0xB, 0xD, 0xC, 0xF, 0x2, 0x10, 0x11, 0x12, 0x19, 0x13, 0x0, 0xE, 0x1};
	WORD setval;
	char temp[5];
	
	if(argc < 2)
	{
		printf("You have to supply a command line argument!\n");
		printf("%ls <new BootNext Value> [/r (for restart)]\n", getProgName(argv[0]));
		getchar();
		return 0;
	}

	TCHAR* str = argv[2];


	WideCharToMultiByte(CP_ACP, NULL, argv[1], sizeof(argv[1]), temp, sizeof(temp), NULL, NULL);
	setval = (WORD)strtol(temp, NULL, 16);

	setToken(SE_SYSTEM_ENVIRONMENT_NAME);
	setToken(SE_SHUTDOWN_NAME);

	setBootNext(setval);

	if(argc == 3 && (wcscmp(_T("/r"), argv[2]) == 0))
	{
		ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_FLAG_PLANNED);
		return 0;
	}

	return 0;
}

TCHAR* getProgName(TCHAR* str)
{
	TCHAR *token = NULL, *nextToken = NULL, *lastRes = NULL;

	token = wcstok_s(str, _T("//\\"), &nextToken);

	while(token != NULL)
	{
		lastRes = token;
		token = wcstok_s(NULL, _T("//\\"), &nextToken);
	}

	return lastRes;

}

bool setToken(TCHAR* str)
{
	TOKEN_PRIVILEGES tp;
	HANDLE handle;
	
	ZeroMemory(&tp, sizeof(tp));
	
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &handle))
	{
		printf("Failed to open process\n");
		return FALSE;
	}
	if(!LookupPrivilegeValue(NULL, str, &tp.Privileges[0].Luid))
	{
		printf("Failed to locate env privilege\n");
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if(!AdjustTokenPrivileges(handle, FALSE, &tp, 0, NULL, 0))
	{
		printf("Failed to adjust privileges");
		return FALSE;
	}

	return TRUE;
}

bool setBootNext(WORD setval)
{
	TCHAR varname[] = _T("BootNext");
	TCHAR VendorGuiD[] = _T("{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}");

	if(!SetFirmwareEnvironmentVariable(varname, VendorGuiD, &setval, sizeof(setval)))
	{
		printf("Could not set value!");
		return FALSE;
	}

	return TRUE;
}