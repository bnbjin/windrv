#pragma once

#include <wdm.h>

DRIVER_DISPATCH DispDevCreate;
DRIVER_DISPATCH DispDevClose;
DRIVER_DISPATCH DispDevCleanup;
DRIVER_DISPATCH DispDevCtl;
DRIVER_UNLOAD DriverUnload;

NTSTATUS
OpenProc(
	_In_  HANDLE Pid,
	_Out_ HANDLE *ProcHandle
	);

VOID
thr_proc(_In_ PVOID StartContext);

#pragma pack(push)
#pragma pack(1)

typedef struct _DEV_EXT {
	PUNICODE_STRING SymLink;
} DEV_EXT, *PDEV_EXT;

#pragma pack(pop)