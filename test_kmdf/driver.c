#include <ntifs.h>
#include <windef.h>

VOID tDriverUnload(PDRIVER_OBJECT pDriverObj)
{
	UNREFERENCED_PARAMETER(pDriverObj);

	LARGE_INTEGER interval;

	interval.QuadPart = 5000 * -10 * 1000;
	KeDelayExecutionThread(KernelMode, FALSE, &interval);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(pDriverObj);
	UNREFERENCED_PARAMETER(reg_path);

	NTSTATUS status = STATUS_SUCCESS;

	//KdBreakPoint();
	//pDriverObj->DriverUnload = tDriverUnload;

	/*for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = GenericHandler;
	}*/

	return status;
}