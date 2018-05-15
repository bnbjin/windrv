#include <stdio.h>
#include <Windows.h>
#include "..\thread_ioctl\shared.h"

int main(int argc, char *argv[])
{
	HANDLE hDev = INVALID_HANDLE_VALUE;
	DWORD dwBytesRead = 0;
	ULONG ulPid = 0;

	hDev = CreateFile(TEXT(DEVNAME_APP), 
					0, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, 
					NULL, OPEN_EXISTING, 
					0, 
					NULL);
	if (INVALID_HANDLE_VALUE == hDev)
	{
		printf("Opening device failed with 0x%08X.", GetLastError());
		return -1;
	}

	ulPid = GetCurrentProcessId();

	(void)DeviceIoControl(hDev, 
						TI_IOCTL_CREATE_THREAD, 
						(LPVOID)&ulPid, 
						sizeof(ulPid), 
						NULL, 
						0, 
						&dwBytesRead, 
						NULL);

	CloseHandle(hDev);

	system("pause");

	return 0;
}