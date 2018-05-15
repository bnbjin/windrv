#pragma once

#include <wdm.h>

#define DEVNAME_NT L"\\Device\\dev_work_item_test"
#define DEVNAME_DOS L"\\DosDevices\\dev_work_item_test" 
#define INTERFACE_CLASS_GUID L"{ 78A1C341 - 4539 - 11d3 - B88D - 00C04FAD5171 }"
#define DEVNAME_APP "\\\\.\\dev_work_item_test"

DRIVER_DISPATCH DispDevCreate;
DRIVER_DISPATCH DispDevClose;
DRIVER_DISPATCH DispDevCleanup;
DRIVER_DISPATCH DispDevCtl;
DRIVER_UNLOAD DriverUnload;

#pragma pack(push)
#pragma pack(1)

typedef struct _DEV_EXT {
	PUNICODE_STRING SymLink;
} DEV_EXT, *PDEV_EXT;

#pragma pack(pop)